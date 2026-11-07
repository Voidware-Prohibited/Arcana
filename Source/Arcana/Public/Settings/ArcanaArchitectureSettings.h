// MIT

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ArcanaArchitectureSettings.generated.h"


// Pre-defined Attribute Sets Arsenal can manage while in Inventory
USTRUCT(BlueprintType)
struct FArcanaArchitectureSettings
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	bool bEOSIntegration {false};

	// True = Integrate with Abraxas, False = Local Mode
	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	bool bAbraxasIntegration {false};

	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	bool bClimaIntegration {false};

	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	bool bChronosIntegration {false};

	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	bool bEchelonIntegration {false};

	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	bool bKairosIntegration {false};

	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	FString AbraxasAddress;

	UPROPERTY(Config, EditAnywhere, Category = "Architecture|EOS")
	FString EOSApiKey;

	UPROPERTY(Config, EditAnywhere, Category = "Architecture|Abraxas")
	FString AbraxasApiKey;
};