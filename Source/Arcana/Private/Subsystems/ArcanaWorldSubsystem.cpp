// MIT


#include "Subsystems/ArcanaWorldSubsystem.h"
#include "HttpModule.h"
#include "Json.h"
#include "Async/Async.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Utility/ArcanaLog.h"

void FAsyncStreamingWorker::DoWork()
{
    OutProximityResults.SetNumUninitialized(PointPositions.Num());
    for (int32 i = 0; i < PointPositions.Num(); ++i)
    {
        bool bIsNear = false;
        for (const FVector& PlayerLoc : PlayerLocations)
        {
            if (FVector::DistSquared(PointPositions[i], PlayerLoc) < RadiusSq)
            {
                bIsNear = true;
                break;
            }
        }
        OutProximityResults[i] = bIsNear;
    }
}

void UArcanaWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    InitializeDataFetch();
}

void UArcanaWorldSubsystem::Deinitialize()
{
    GetWorld()->GetTimerManager().ClearTimer(EvaluationTimerHandle);
    MasterRegistry.Empty();
    CompiledDefinitions.Empty();
    Super::Deinitialize();
}

void UArcanaWorldSubsystem::InitializeDataFetch()
{
    bDefinitionsLoaded = false;
    if (BackendMode == EDataFetchMode::LocalDataTable)
    {
        LoadFromLocalDataTable();
    }
    else
    {
        FetchFromRemoteAPI();
    }
}

void UArcanaWorldSubsystem::LoadFromLocalDataTable()
{
    LocalSpawnDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DynamicSpawner/DT_SpawnableItems.DT_SpawnableItems'")));
    if (!LocalSpawnDataTable) return;

    TArray<FSmartSpawnRow*> Rows;
    LocalSpawnDataTable->GetAllRows(TEXT("Local Mode Fetch"), Rows);
    
    for (const FSmartSpawnRow* Row : Rows)
    {
        if (!Row) continue;
        FSpawnableItemDefinition NewDef;
        NewDef.ActorClass = Row->ActorClass;
        NewDef.ItemTag = Row->ItemTag;
        NewDef.RarityTag = Row->RarityTag;
        NewDef.ZoneTypeTag = Row->ZoneTypeTag;
        NewDef.GlobalMaxCount = Row->GlobalMaxCount;
        CompiledDefinitions.Add(NewDef);
    }
    FinalizeDefinitionsLoad();
}

void UArcanaWorldSubsystem::FetchFromRemoteAPI()
{
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http) return;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UArcanaWorldSubsystem::OnRemoteResponseReceived);
    Request->SetURL(RemoteDatabaseURL);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Server-Spawning-Agent"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->ProcessRequest();
}

void UArcanaWorldSubsystem::OnRemoteResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
        UE_LOG(LogArcana, Error, TEXT("[Spawner] Database API request failed. Falling back to local configuration paths."));
        LoadFromLocalDataTable();
        return;
    }

    FString ContentPayload = Response->GetContentAsString();
    
    // Parse JSON safely on an isolated thread to eliminate hitching on the main thread
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, ContentPayload]()
    {
        ProcessRawJsonPayload(ContentPayload);
    });
}

void UArcanaWorldSubsystem::ProcessRawJsonPayload(const FString& JsonString)
{
    TSharedPtr<FJsonObject> RootJsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, RootJsonObject) && RootJsonObject.IsValid())
    {
        TArray<TSharedPtr<FJsonValue>> JsonItemsArray = RootJsonObject->GetArrayField(TEXT("items"));
        TArray<FSpawnableItemDefinition> TempDefinitions;

        for (const TSharedPtr<FJsonValue>& Value : JsonItemsArray)
        {
            TSharedPtr<FJsonObject> ItemObj = Value->AsObject();
            if (!ItemObj.IsValid()) continue;

            FSpawnableItemDefinition Definition;
            
            // Extract soft class pointers using explicit workspace path strings
            FString ClassPathString = ItemObj->GetStringField(TEXT("actorClassPath"));
            Definition.ActorClass = TSoftClassPtr<AActor>(FSoftObjectPath(ClassPathString));
            
            Definition.ItemTag = FGameplayTag::RequestGameplayTag(FName(*ItemObj->GetStringField(TEXT("itemTag"))));
            Definition.RarityTag = FGameplayTag::RequestGameplayTag(FName(*ItemObj->GetStringField(TEXT("rarityTag"))));
            Definition.ZoneTypeTag = FGameplayTag::RequestGameplayTag(FName(*ItemObj->GetStringField(TEXT("zoneTypeTag"))));
            Definition.GlobalMaxCount = ItemObj->GetIntegerField(TEXT("globalMaxCount"));

            TempDefinitions.Add(Definition);
        }

        // Return definitions safely to the Game Thread
        AsyncTask(ENamedThreads::GameThread, [this, TempDefinitions]()
        {
            this->CompiledDefinitions = TempDefinitions;
            this->FinalizeDefinitionsLoad();
        });
    }
}

void UArcanaWorldSubsystem::FinalizeDefinitionsLoad()
{
    for (const FSpawnableItemDefinition& Def : CompiledDefinitions)
    {
        if (Def.GlobalMaxCount >= 0)
        {
            GlobalLimits.Add(Def.ItemTag, Def.GlobalMaxCount);
            CurrentGlobalCounts.Add(Def.ItemTag, 0);
        }
    }

    bDefinitionsLoaded = true;
    
    // Broadcast initialization complete to notify volume components to register
    OnDefinitionsReady.Broadcast();

    GetWorld()->GetTimerManager().SetTimer(EvaluationTimerHandle, this, &UArcanaWorldSubsystem::RunParallelStreamingCheck, 0.10f, true);
}

void UArcanaWorldSubsystem::RegisterDirectPoints(uint32 SourceID, const TArray<FTransform>& Transforms, const FGameplayTagContainer& ZoneFilters, const FGameplayTagContainer& RarityFilters, float MinDelay, float MaxDelay)
{
    if (!bDefinitionsLoaded) return;

    for (const FTransform& Form : Transforms)
    {
        const FSpawnableItemDefinition* Selection = SelectRandomRow(ZoneFilters, RarityFilters);
        if (!Selection) continue;

        if (GlobalLimits.Contains(Selection->ItemTag))
        {
            if (CurrentGlobalCounts[Selection->ItemTag] >= GlobalLimits[Selection->ItemTag]) continue;
            CurrentGlobalCounts[Selection->ItemTag]++;
        }

        FPackedSpawnInstance Point;
        Point.Position = Form.GetLocation();
        Point.Rotation = Form.Rotator();
        Point.AssociatedID = SourceID;
        Point.ItemTag = Selection->ItemTag;
        Point.UniqueInstanceHash = HashCombine(SourceID, GetTypeHash(Point.Position));
        Point.CurrentState = ESpawnState::Unspawned;
        Point.PhysicalActor = nullptr;
        Point.MinRespawnDelay = MinDelay;
        Point.MaxRespawnDelay = MaxDelay;

        MasterRegistry.Add(Point);
    }
}

void UArcanaWorldSubsystem::UnregisterSource(uint32 SourceID)
{
    for (int32 i = MasterRegistry.Num() - 1; i >= 0; --i)
    {
        if (MasterRegistry[i].AssociatedID == SourceID)
        {
            if (MasterRegistry[i].PhysicalActor.IsValid())
            {
                MasterRegistry[i].PhysicalActor->Destroy();
            }
            if (CurrentGlobalCounts.Contains(MasterRegistry[i].ItemTag))
            {
                CurrentGlobalCounts[MasterRegistry[i].ItemTag] = FMath::Max(0, CurrentGlobalCounts[MasterRegistry[i].ItemTag] - 1);
            }
            MasterRegistry.RemoveAtSwap(i, 1, EAllowShrinking::No);
        }
    }
}

void UArcanaWorldSubsystem::RunParallelStreamingCheck()
{
    if (bWorkerThreadActive || MasterRegistry.Num() == 0 || !bDefinitionsLoaded) return;
    bWorkerThreadActive = true;

    // Step 1: Collect player vectors on game-thread array parameters
    TArray<FVector, TInlineAllocator<8>> InlinePlayerPositions;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (APawn* P = PC->GetPawn()) InlinePlayerPositions.Add(P->GetActorLocation());
        }
    }

    // Step 2: Convert to a regular flat TArray to match Constructor signature exactly
    TArray<FVector> ThreadSafePlayerLocations;
    ThreadSafePlayerLocations.Reserve(InlinePlayerPositions.Num());
    for(const FVector& Loc : InlinePlayerPositions)
    {
        ThreadSafePlayerLocations.Add(Loc);
    }

    // Step 3: Package spatial points array 
    TArray<FVector> VectorPositions;
    VectorPositions.SetNumUninitialized(MasterRegistry.Num());
    for (int32 i = 0; i < MasterRegistry.Num(); ++i)
    {
        VectorPositions[i] = MasterRegistry[i].Position;
    }

    // Step 4: Dispatch safely without compilation signature mismatch exceptions
    FAsyncTask<FAsyncStreamingWorker>* StreamingTask = new FAsyncTask<FAsyncStreamingWorker>(
        ThreadSafePlayerLocations, 
        VectorPositions, 
        StreamRadiusSq
    );
    
    TWeakObjectPtr<UArcanaWorldSubsystem> WeakSelf(this);
    StreamingTask->StartBackgroundTask();

    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [StreamingTask, WeakSelf]()
    {
        StreamingTask->EnsureCompletion();
        TArray<bool> FinishedResults = StreamingTask->GetTask().OutProximityResults;
        delete StreamingTask;

        AsyncTask(ENamedThreads::GameThread, [WeakSelf, FinishedResults]()
        {
            if (UArcanaWorldSubsystem* Self = WeakSelf.Get())
            {
                Self->ApplyParallelStreamingResults(FinishedResults);
            }
        });
    });
}

void UArcanaWorldSubsystem::ApplyParallelStreamingResults(TArray<bool> ProximityResults)
{
    bWorkerThreadActive = false;
    if (MasterRegistry.Num() != ProximityResults.Num()) return;

    for (int32 i = 0; i < MasterRegistry.Num(); ++i)
    {
        FPackedSpawnInstance& Point = MasterRegistry[i];
        bool bIsNear = ProximityResults[i];

        if (bIsNear && Point.CurrentState == ESpawnState::Unspawned)
        {
            Point.CurrentState = ESpawnState::AsyncLoading;
            for (const FSpawnableItemDefinition& Def : CompiledDefinitions)
            {
                if (Def.ItemTag == Point.ItemTag)
                {
                    RequestAsyncMaterialize(i, Def.ActorClass);
                    break;
                }
            }
        }
        else if (!bIsNear && Point.CurrentState == ESpawnState::Materialized)
        {
            if (Point.PhysicalActor.IsValid())
            {
                Point.Position = Point.PhysicalActor->GetActorLocation();
                Point.Rotation = Point.PhysicalActor->GetActorRotation();
                Point.PhysicalActor->Destroy();
            }
            Point.CurrentState = ESpawnState::Unspawned;
            Point.PhysicalActor.Reset();
        }
    }
}

void UArcanaWorldSubsystem::RequestAsyncMaterialize(int32 TargetIndex, TSoftClassPtr<AActor> ClassPtr)
{
    TWeakObjectPtr<UArcanaWorldSubsystem> WeakSelf(this);
    
    StreamableManager.RequestAsyncLoad(ClassPtr.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([WeakSelf, TargetIndex, ClassPtr]()
    {
        UArcanaWorldSubsystem* Self = WeakSelf.Get();
        if (!Self || !Self->MasterRegistry.IsValidIndex(TargetIndex)) return;

        FPackedSpawnInstance& TargetPoint = Self->MasterRegistry[TargetIndex];
        if (TargetPoint.CurrentState != ESpawnState::AsyncLoading) return;

        if (UClass* LoadedClass = ClassPtr.Get())
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            
            FTransform SpawnForm(TargetPoint.Rotation, TargetPoint.Position, FVector::OneVector);
            AActor* MaterializedActor = Self->GetWorld()->SpawnActor<AActor>(LoadedClass, SpawnForm, Params);
            
            if (MaterializedActor)
            {
                TargetPoint.PhysicalActor = MaterializedActor;
                TargetPoint.CurrentState = ESpawnState::Materialized;
                MaterializedActor->OnDestroyed.AddUniqueDynamic(Self, &UArcanaWorldSubsystem::OnPhysicalActorDestroyed);
            }
            else
            {
                TargetPoint.CurrentState = ESpawnState::Unspawned;
            }
        }
    }));
}

void UArcanaWorldSubsystem::OnPhysicalActorDestroyed(AActor* DestroyedActor)
{
    if (!DestroyedActor) return;

    for (int32 i = 0; i < MasterRegistry.Num(); ++i)
    {
        if (MasterRegistry[i].PhysicalActor.Get() == DestroyedActor)
        {
            FPackedSpawnInstance& Point = MasterRegistry[i];
            Point.CurrentState = ESpawnState::PendingRespawn;
            Point.PhysicalActor.Reset();

            if (CurrentGlobalCounts.Contains(Point.ItemTag))
            {
                CurrentGlobalCounts[Point.ItemTag] = FMath::Max(0, CurrentGlobalCounts[Point.ItemTag] - 1);
            }

            float RandomDelay = FMath::FRandRange(Point.MinRespawnDelay, Point.MaxRespawnDelay);
            FTimerHandle DelayHandle;
            
            GetWorld()->GetTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateUObject(
                this, &UArcanaWorldSubsystem::ExecuteRespawnLogic, Point.UniqueInstanceHash, Point.ItemTag, Point.AssociatedID, Point.MinRespawnDelay, Point.MaxRespawnDelay
            ), RandomDelay, false);
            break;
        }
    }
}

void UArcanaWorldSubsystem::ExecuteRespawnLogic(uint32 InstanceHash, FGameplayTag ItemTag, uint32 SourceID, float MinDelay, float MaxDelay)
{
    for (int32 i = 0; i < MasterRegistry.Num(); ++i)
    {
        if (MasterRegistry[i].UniqueInstanceHash == InstanceHash)
        {
            if (GlobalLimits.Contains(ItemTag) && CurrentGlobalCounts[ItemTag] >= GlobalLimits[ItemTag])
            {
                FTimerHandle RequeueHandle;
                GetWorld()->GetTimerManager().SetTimer(RequeueHandle, FTimerDelegate::CreateUObject(
                    this, &UArcanaWorldSubsystem::ExecuteRespawnLogic, InstanceHash, ItemTag, SourceID, MinDelay, MaxDelay
                ), 4.0f, false);
                return;
            }

            if (GlobalLimits.Contains(ItemTag)) CurrentGlobalCounts[ItemTag]++;
            MasterRegistry[i].CurrentState = ESpawnState::Unspawned;
            break;
        }
    }
}

const FSpawnableItemDefinition* UArcanaWorldSubsystem::SelectRandomRow(const FGameplayTagContainer& Zones, const FGameplayTagContainer& Rarities) const
{
    TArray<const FSpawnableItemDefinition*> Pool;
    for (const FSpawnableItemDefinition& Def : CompiledDefinitions)
    {
        if (Zones.HasTag(Def.ZoneTypeTag) && Rarities.HasTag(Def.RarityTag))
        {
            Pool.Add(&Def);
        }
    }
    return Pool.Num() > 0 ? Pool[FMath::RandRange(0, Pool.Num() - 1)] : nullptr;
}
