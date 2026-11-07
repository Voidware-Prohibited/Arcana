#pragma once

#include "StateTreeTaskBase.h"
#include "SmartObjectComponent.h"
#include "ContextualAnimTypes.h"
#include "MotionWarpingComponent.h"
#include "StateTreeTypes.h"
#include "ArcanaStateTreeTask_PlayContextualAnim.generated.h"

class UContextualAnimSceneAsset;

USTRUCT(BlueprintType)
struct ARCANA_API FArcanaStateTreeTask_PlayContextualAnimInputs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Interactor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> SmartObjectActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UContextualAnimSceneAsset> SceneAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Animation")
	FName InteractorRole = TEXT("Interactor");

	UPROPERTY(EditAnywhere, Category = "Animation")
	FName SmartObjectRole = TEXT("SmartObject");
};

// Persistent instance data mapping to track active scenes across frames
USTRUCT()
struct FArcanaStateTreeTask_PlayContextualAnimInstanceData : public FArcanaStateTreeTask_PlayContextualAnimInputs
{
	GENERATED_BODY()

	// Internal tracker to know if we successfully initiated the scene on EnterState
	UPROPERTY()
	bool bSceneWasStarted = false;
};

USTRUCT(BlueprintType)
struct ARCANA_API FArcanaStateTreeTask_PlayContextualAnimTask : public FStateTreeTaskBase
{
	GENERATED_BODY()

	FArcanaStateTreeTask_PlayContextualAnimTask() = default;

	using FInstanceDataType = FArcanaStateTreeTask_PlayContextualAnimInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FArcanaStateTreeTask_PlayContextualAnimInstanceData::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};