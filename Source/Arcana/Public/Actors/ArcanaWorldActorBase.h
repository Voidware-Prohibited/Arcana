// MIT

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ContextualAnimSceneActorComponent.h"
#include "SmartObjectComponent.h"
#include "GameFramework/Actor.h"
#include "ContextualAnimSceneAsset.h"
#include "Interfaces/ArcanaWorldActorInterface.h"
#include "ArcanaWorldActorBase.generated.h"

class UContextualAnimSceneAsset;
class UContextualAnimSceneActorComponent;

UCLASS()
class ARCANA_API AArcanaWorldActorBase : public AActor, public IAbilitySystemInterface, public IArcanaWorldActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArcanaWorldActorBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The component that registers this actor as a Smart Object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SmartObject")
	TObjectPtr<USmartObjectComponent> SmartObjectComponent;
	
	// The component that registers this actor as a Smart Object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SmartObject")
	TObjectPtr<UContextualAnimSceneActorComponent> ContextualAnimationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UContextualAnimSceneAsset> ContextualAnimationSceneAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UContextualAnimRolesAsset> ContextualAnimationRolesAsset;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
