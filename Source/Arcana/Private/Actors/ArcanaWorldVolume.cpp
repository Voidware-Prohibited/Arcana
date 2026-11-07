// MIT


#include "Actors/ArcanaWorldVolume.h"
#include "Subsystems/ArcanaWorldSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BrushComponent.h"

// Sets default values
AArcanaWorldVolume::AArcanaWorldVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AArcanaWorldVolume::BeginPlay()
{
	Super::BeginPlay();
	VolumeInternalID = GetTypeHash(GetOwner()->GetName() + GetOwner()->GetActorLocation().ToString());
    
	if (GetOwner()->HasAuthority())
	{
		if (UArcanaWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UArcanaWorldSubsystem>())
		{
			if (Subsystem->AreDefinitionsLoaded())
			{
				EvaluateVolumePoints();
			}
			else
			{
				// Dynamic callback handling for remote endpoints
				Subsystem->OnDefinitionsReady.AddUObject(this, &AArcanaWorldVolume::EvaluateVolumePoints);
			}
		}
	}
	
}

// Called every frame
void AArcanaWorldVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AArcanaWorldVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UArcanaWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UArcanaWorldSubsystem>())
	{
		Subsystem->UnregisterSource(VolumeInternalID);
	}
	Super::EndPlay(EndPlayReason);
}

void AArcanaWorldVolume::EvaluateVolumePoints()
{
	UArcanaWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UArcanaWorldSubsystem>();
	UBrushComponent* BrushComp = GetOwner()->FindComponentByClass<UBrushComponent>();
	if (!Subsystem || !BrushComp) return;

	int32 RandomTargetCount = UKismetMathLibrary::RandomIntegerInRange(MinSpawns, MaxSpawns);
	TArray<FTransform> StructuralPoints;

	FVector BoundsOrigin = BrushComp->Bounds.Origin;
	FVector BoxExtents = BrushComp->Bounds.BoxExtent;

	for (int32 i = 0; i < RandomTargetCount; ++i)
	{
		FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(BoundsOrigin, BoxExtents);
		StructuralPoints.Add(FTransform(FRotator::ZeroRotator, RandomPoint, FVector::OneVector));
	}

	Subsystem->RegisterDirectPoints(VolumeInternalID, StructuralPoints, ZoneTags, RarityTags, MinRespawnDelay, MaxRespawnDelay);
}
