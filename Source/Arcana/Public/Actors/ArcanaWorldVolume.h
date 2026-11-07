// MIT

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actors/ProceduralVolumeWithFalloffActor.h"
#include "ArcanaWorldVolume.generated.h"

UCLASS()
class ARCANA_API AArcanaWorldVolume : public AProceduralVolumeWithFalloffActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArcanaWorldVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void EndPlay(EEndPlayReason::Type EndPlayReason);
	void EvaluateVolumePoints();
	uint32 VolumeInternalID;

	FGameplayTagContainer ZoneTags;
	FGameplayTagContainer RarityTags;
	int MinSpawns;
	int MaxSpawns;
	int MinRespawnDelay;
	int MaxRespawnDelay;
	
};
