// MIT


#include "Actors/ArcanaWorldActorBase.h"
#include "AbilitySystemComponent.h"


// Sets default values
AArcanaWorldActorBase::AArcanaWorldActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the mesh component and set it as root
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	// RootComponent = StaticMeshComponent;
	SetRootComponent(StaticMeshComponent);

	// Create and attach the Smart Object Component
	SmartObjectComponent = CreateDefaultSubobject<USmartObjectComponent>(TEXT("SmartObjectComponent"));
	SmartObjectComponent->SetupAttachment(RootComponent);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ContextualAnimationComponent = CreateDefaultSubobject<UContextualAnimSceneActorComponent>(TEXT("ContextualAnimComponent"));
}

UAbilitySystemComponent* AArcanaWorldActorBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AArcanaWorldActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArcanaWorldActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

