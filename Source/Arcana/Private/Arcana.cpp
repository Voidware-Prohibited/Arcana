// Copyright Epic Games, Inc. All Rights Reserved.

#include "Arcana.h"
#include "Utility/ArcanaLog.h"

#if WITH_EDITOR
#include "MessageLogModule.h"
#endif

#define LOCTEXT_NAMESPACE "FArcanaModule"

void FArcanaModule::StartupModule()
{
	IModuleInterface::StartupModule();
	
#if WITH_EDITOR
	auto& MessageLog{FModuleManager::LoadModuleChecked<FMessageLogModule>(FName{TEXTVIEW("MessageLog")})};

	FMessageLogInitializationOptions MessageLogOptions;
	MessageLogOptions.bShowFilters = true;
	MessageLogOptions.bAllowClear = true;
	MessageLogOptions.bDiscardDuplicates = true;

	MessageLog.RegisterLogListing(ArcanaLog::MessageLogName, LOCTEXT("MessageLogLabel", "Arcana"), MessageLogOptions);
#endif
}

void FArcanaModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArcanaModule, Arcana)