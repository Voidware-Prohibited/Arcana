#include "StateTree/ArcanaStateTreeTask_PlayContextualAnim.h"
#include "StateTreeExecutionContext.h"
#include "ContextualAnimSceneActorComponent.h"

EStateTreeRunStatus FArcanaStateTreeTask_PlayContextualAnimTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FArcanaStateTreeTask_PlayContextualAnimInstanceData& InstanceData = Context.GetInstanceData<FArcanaStateTreeTask_PlayContextualAnimInstanceData>(*this);

	if (!InstanceData.SceneAsset || !InstanceData.Interactor || !InstanceData.SmartObjectActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	UContextualAnimSceneActorComponent* InteractorComp = InstanceData.Interactor->FindComponentByClass<UContextualAnimSceneActorComponent>();
	if (!InteractorComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ContextualAnim Task failed: Interactor missing UContextualAnimSceneActorComponent."));
		return EStateTreeRunStatus::Failed;
	}

	// Modern 5.7 Bindings setup: Construct explicitly from Asset and Section index (0 = Default)
	FContextualAnimSceneBindings TargetBindings(*InstanceData.SceneAsset, 0, 0);

	// FContextualAnimSceneBindings TargetBindings;
	// TargetBindings.TryCreateBindings(*InstanceData.SceneAsset.Get(), 0, 0);
	// 
	// if (FContextualAnimSceneBindings::TryMakeBindings(*InstanceData.SceneAsset, 0, 0))
	// {
	// 	TargetBindings.BindActorToRole(*InstanceData.Interactor, InstanceData.InteractorRole, 0, 0);
	// 	TargetBindings.BindActorToRole(*InstanceData.SmartObjectActor, InstanceData.SmartObjectRole, 0, 0);
	// }
    
	// Explicitly add your primary context actors to their designated role definitions
	TargetBindings.BindActorToRole(*InstanceData.Interactor, InstanceData.InteractorRole, 0, 0);
	TargetBindings.BindActorToRole(*InstanceData.SmartObjectActor, InstanceData.SmartObjectRole, 0, 0);

	// Start scene execution (Returns bool natively in 5.4+)
	const bool bSceneStarted = InteractorComp->StartContextualAnimScene(TargetBindings);

	if (bSceneStarted)
	{
		InstanceData.bSceneWasStarted = true;
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FArcanaStateTreeTask_PlayContextualAnimTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    const FArcanaStateTreeTask_PlayContextualAnimInstanceData& InstanceData = Context.GetInstanceData<FArcanaStateTreeTask_PlayContextualAnimInstanceData>(*this);

	UContextualAnimSceneActorComponent* InteractorComp = InstanceData.Interactor->FindComponentByClass<UContextualAnimSceneActorComponent>();

	// 1. Get the actor running this State Tree
	AActor* OwnerActor = Cast<AActor>(InstanceData.SmartObjectActor);
	if (!OwnerActor) return EStateTreeRunStatus::Failed;

	// 2. Fetch the Contextual Anim Scene Actor Component from the actor
	UContextualAnimSceneActorComponent* SceneActorComp = OwnerActor->FindComponentByClass<UContextualAnimSceneActorComponent>();
	if (!SceneActorComp) return EStateTreeRunStatus::Failed;

	// 3. Verify if the actor is joined to an active scene
	if (SceneActorComp->IsInActiveScene())
	{
		// 4. Retrieve the bindings which hold the data of the running scene
		const FContextualAnimSceneBindings& Bindings = SceneActorComp->GetBindings();

		// 5. Verify validity and match against your specific expected asset
		// (Assuming you have a target UContextualAnimSceneAsset* ExpectedAsset referenced)
		if (Bindings.IsValid() && Bindings.GetSceneAsset() == InstanceData.SceneAsset)
		{
			// 6. Check if the scene is playing by evaluating the component's internal playback status
			if (SceneActorComp->HasBegunPlay()) 
			{
				UE_LOG(LogTemp, Log, TEXT("Target scene asset is valid, bound, and actively playing."));
				return EStateTreeRunStatus::Running;
			}
		}
	}
	
	// When the Montage completes or stops playing, IsInScene() automatically becomes false.
	// TODO Determine Scene Instance Playing State
	if (!InteractorComp || !InteractorComp->SceneAsset)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	// OR
	// if (!InteractorComp || !InstanceData.ActiveSceneInstance)
	// {
	// 	return EStateTreeRunStatus::Succeeded;
	// }

	return EStateTreeRunStatus::Running;
}

void FArcanaStateTreeTask_PlayContextualAnimTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FArcanaStateTreeTask_PlayContextualAnimInstanceData& InstanceData = Context.GetInstanceData<FArcanaStateTreeTask_PlayContextualAnimInstanceData>(*this);

	UContextualAnimSceneActorComponent* SceneActorComp = InstanceData.SmartObjectActor->FindComponentByClass<UContextualAnimSceneActorComponent>();
	
    // Gracefully terminate the scene if exiting early or cleaning up
    if (SceneActorComp->IsInActiveScene() && InstanceData.Interactor)
    {
        if (UContextualAnimSceneActorComponent* InteractorComp = InstanceData.Interactor->FindComponentByClass<UContextualAnimSceneActorComponent>())
        {
            InteractorComp->EarlyOutContextualAnimScene();
        }
    	InstanceData.bSceneWasStarted = false;
    }
}