#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ArcanaVirtualizedSpawnTypes.generated.h"

UENUM(BlueprintType)
enum class ESpawnState : uint8
{
    Unspawned = 0,
    AsyncLoading = 1,
    Materialized = 2,
    PendingRespawn = 3
};

UENUM(BlueprintType)
enum class EDataFetchMode : uint8
{
    LocalDataTable = 0,
    RemoteDatabaseAPI = 1
};

// Unified run-time definition schema
USTRUCT(BlueprintType)
struct FSpawnableItemDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    TSoftClassPtr<AActor> ActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    FGameplayTag ItemTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    FGameplayTag RarityTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    FGameplayTag ZoneTypeTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    int32 GlobalMaxCount = -1;
};

// Kept purely for Local Mode fallback operation inside Editor/Engine content browsers
USTRUCT(BlueprintType)
struct FSmartSpawnRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    TSoftClassPtr<AActor> ActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    FGameplayTag ItemTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    FGameplayTag RarityTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    FGameplayTag ZoneTypeTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    int32 GlobalMaxCount = -1;
};

// STRUCT_ALIGNMENT_VALIDATION(FPackedSpawnInstance);
struct alignas(16) FPackedSpawnInstance
{
    FVector Position;
    uint32 AssociatedID;
    FRotator Rotation;
    FGameplayTag ItemTag;
    uint32 UniqueInstanceHash;
    ESpawnState CurrentState;
    
    TWeakObjectPtr<AActor> PhysicalActor;
    float MinRespawnDelay;
    float MaxRespawnDelay;
};