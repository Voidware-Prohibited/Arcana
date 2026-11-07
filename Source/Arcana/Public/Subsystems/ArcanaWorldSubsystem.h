// MIT

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/ArcanaVirtualizedSpawnTypes.h"
#include "Engine/StreamableManager.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Async/AsyncWork.h"
#include "ArcanaWorldSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSpawnDefinitionsReady);

class FAsyncStreamingWorker : public FNonAbandonableTask
{
public:
	FAsyncStreamingWorker(const TArray<FVector>& InPlayerLocations, const TArray<FVector>& InPointPositions, float InRadiusSq)
		: PlayerLocations(InPlayerLocations), PointPositions(InPointPositions), RadiusSq(InRadiusSq) {}

	TArray<FVector> PlayerLocations;
	TArray<FVector> PointPositions;
	TArray<bool> OutProximityResults;
	float RadiusSq;

	void DoWork();
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncStreamingWorker, STATGROUP_ThreadPoolAsyncTasks); }
};

/**
 * 
 */
UCLASS()
class ARCANA_API UArcanaWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize() override;

    void RegisterDirectPoints(uint32 SourceID, const TArray<FTransform>& Transforms, const FGameplayTagContainer& ZoneFilters, const FGameplayTagContainer& RarityFilters, float MinDelay, float MaxDelay);
    void UnregisterSource(uint32 SourceID);

    // Event hooks to delay initial spatial volume allocations until data fetches complete
    FOnSpawnDefinitionsReady OnDefinitionsReady;
    bool AreDefinitionsLoaded() const { return bDefinitionsLoaded; }

private:
    UPROPERTY(EditDefaultsOnly, Category = "Spawner Settings")
    EDataFetchMode BackendMode = EDataFetchMode::RemoteDatabaseAPI;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner Settings")
    FString RemoteDatabaseURL = TEXT("yourgame.com");

    UPROPERTY()
    UDataTable* LocalSpawnDataTable;

    FStreamableManager StreamableManager;
    FTimerHandle EvaluationTimerHandle;
    bool bWorkerThreadActive = false;
    bool bDefinitionsLoaded = false;

    TArray<FPackedSpawnInstance> MasterRegistry;
    TArray<FSpawnableItemDefinition> CompiledDefinitions;
    
    TMap<FGameplayTag, int32> GlobalLimits;
    TMap<FGameplayTag, int32> CurrentGlobalCounts;

    const float StreamRadiusSq = 144000000.0f; // 120 Meters

    void InitializeDataFetch();
    void LoadFromLocalDataTable();
    void FetchFromRemoteAPI();
    void OnRemoteResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void ProcessRawJsonPayload(const FString& JsonString);
    void FinalizeDefinitionsLoad();

    void RunParallelStreamingCheck();
    void ApplyParallelStreamingResults(TArray<bool> ProximityResults);
    
    const FSpawnableItemDefinition* SelectRandomRow(const FGameplayTagContainer& Zones, const FGameplayTagContainer& Rarities) const;
    void RequestAsyncMaterialize(int32 TargetIndex, TSoftClassPtr<AActor> ClassPtr);
    
    UFUNCTION()
    void OnPhysicalActorDestroyed(AActor* DestroyedActor);
    void ExecuteRespawnLogic(uint32 InstanceHash, FGameplayTag ItemTag, uint32 SourceID, float MinDelay, float MaxDelay);
};
