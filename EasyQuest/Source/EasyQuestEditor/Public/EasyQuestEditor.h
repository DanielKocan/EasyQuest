#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyle.h"

// Module initialization, registers custom asset type with Unreal
class FEasyQuestEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FSlateStyleSet> _styleSet = nullptr;
	TSharedPtr<struct FEasyQuestPinFactory> _pinFacotry = nullptr;
	TSharedPtr<struct FGraphPanelNodeFactory> _nodeFactory;
};
