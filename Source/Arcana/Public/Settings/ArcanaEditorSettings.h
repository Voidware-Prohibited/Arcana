// MIT

#pragma once

#include "CoreMinimal.h"
#include "ArcanaArchitectureSettings.h"
#include "Engine/DeveloperSettings.h"
#include "ArcanaEditorSettings.generated.h"

/**
 * 
 */
UCLASS(config = Arsenal, DefaultConfig)
class ARCANA_API UArcanaEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UArcanaEditorSettings();
	
	UPROPERTY(Config, EditAnywhere, Category = "Architecture")
	FArcanaArchitectureSettings ArchitectureSettings;

#if WITH_EDITOR
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FText GetSectionText() const override { return NSLOCTEXT("Arcana", "ArcanaSectionText", "Arcana Settings"); }
	virtual FText GetSectionDescription() const override { return NSLOCTEXT("Arcana", "ArcanaSectionDesc", "Configure Arcana Editor Settings"); }
#endif
};
