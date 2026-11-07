#pragma once

#include "NativeGameplayTags.h"
#include "ArcanaWorldActorInterface.generated.h"

UINTERFACE(Blueprintable)
class UArcanaWorldActorInterface : public UInterface {
	GENERATED_BODY()
};

class ARCANA_API IArcanaWorldActorInterface {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Arcana World Actor Interface")
	FText GetActorName();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Arcana World Actor Interface")
	void GetInventory();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Arcana World Actor Interface")
	void SearchCompartment();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Arcana World Actor Interface")
	void DropLoot();
};