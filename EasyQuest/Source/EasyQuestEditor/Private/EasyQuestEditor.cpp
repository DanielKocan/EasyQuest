// Copyright Epic Games, Inc. All Rights Reserved.

#include "EasyQuestEditor.h"
#include "EasyQuestAction.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EdGraphUtilities.h"
#include "KismetPins/SGraphPinColor.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorModule.h"
#include "SEasyQuestGraphNode.h" 
#include "EasyQuestGraphNode.h"

#define LOCTEXT_NAMESPACE "FEasyQuestEditorModule"

class SEasyQuestGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SEasyQuestGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& inArgs, UEdGraphPin* inGraphPinObj) {
		SGraphPin::Construct(SGraphPin::FArguments(), inGraphPinObj);
	}

protected:
	virtual FSlateColor GetPinColor() const override {
		return FSlateColor(FLinearColor(0.2f, 1.0f, 0.2f));
	}
};

class SEasyQuestStartGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SEasyQuestStartGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& inArgs, UEdGraphPin* inGraphPinObj) {
		SGraphPin::Construct(SGraphPin::FArguments(), inGraphPinObj);
	}

protected:
	virtual FSlateColor GetPinColor() const override {
		return FSlateColor(FLinearColor(1.0f, 0.2f, 0.2f));
	}
};

class SEasyQuestEndGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SEasyQuestEndGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& inArgs, UEdGraphPin* inGraphPinObj) {
		SGraphPin::Construct(SGraphPin::FArguments(), inGraphPinObj);
	}

protected:
	virtual FSlateColor GetPinColor() const override {
		return FSlateColor(FLinearColor(0.2f, 0.2f, 1.0f));
	}
};

struct FEasyQuestPinFactory : public FGraphPanelPinFactory {
public:
	virtual ~FEasyQuestPinFactory() {}
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* pin) const override { //It will get called when you create ANY pin, even not from our assest. We need a way to indetify it is our pin
		if (FName(TEXT("EasyQuestPin")) == pin->PinType.PinSubCategory) {
			return SNew(SEasyQuestGraphPin, pin);
		} else if (FName(TEXT("StartPin")) == pin->PinType.PinSubCategory) { //here configure how it can look
			return SNew(SEasyQuestStartGraphPin, pin);
		} else if (FName(TEXT("EndPin")) == pin->PinType.PinSubCategory) { //here configure how it can look
			return SNew(SEasyQuestEndGraphPin, pin);
		}
		return nullptr;
	}
};

// NODE FACTORY
struct FEasyQuestNodeFactory : public FGraphPanelNodeFactory
{
public:
	virtual ~FEasyQuestNodeFactory() {}

	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UEasyQuestGraphNode* QuestNode = Cast<UEasyQuestGraphNode>(Node))
		{
			return SNew(SEasyQuestGraphNode, QuestNode);
		}
		return nullptr;
	}
};

void FEasyQuestEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IAssetTools& assetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type assetType = assetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("EasyQuest")), FText::FromString("Easy Quest")); // Making category to get it in context menu
	TSharedPtr<EasyQuestAction> easyQuestAction = MakeShareable(new EasyQuestAction(assetType)); // made an instance of custom action 
	assetToolsModule.RegisterAssetTypeActions(easyQuestAction.ToSharedRef()); // Register type action

	_styleSet = MakeShareable(new FSlateStyleSet(TEXT("EasyQuestEditorStyle")));
	TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin("EasyQuest");
	FString contentDir = plugin->GetContentDir();
	_styleSet->SetContentRoot(contentDir);

	FSlateImageBrush* thumbnailBrush = new FSlateImageBrush(_styleSet->RootToContentDir(TEXT("AssetIcon"), TEXT(".png")), FVector2D(128.0, 128.0));
	FSlateImageBrush* iconBrush = new FSlateImageBrush(_styleSet->RootToContentDir(TEXT("AssetIcon"), TEXT(".png")), FVector2D(128.0, 128.0));
	FSlateImageBrush* nodeAddPinIcon = new FSlateImageBrush(_styleSet->RootToContentDir(TEXT("NodeAddPinIcon"), TEXT(".png")), FVector2D(128.0f, 128.0f));
	FSlateImageBrush* nodeDeletePinIcon = new FSlateImageBrush(_styleSet->RootToContentDir(TEXT("NodeDeletePinIcon"), TEXT(".png")), FVector2D(128.0f, 128.0f));
	FSlateImageBrush* nodeDeleteNodeIcon = new FSlateImageBrush(_styleSet->RootToContentDir(TEXT("NodeDeleteNodeIcon"), TEXT(".png")), FVector2D(128.0f, 128.0f));

	_styleSet->Set(TEXT("ClassThumbnail.EasyQuestAsset"), thumbnailBrush);
	_styleSet->Set(TEXT("ClassIcon.EasyQuestAsset"), iconBrush);
	_styleSet->Set(TEXT("EasyQuestEditor.NodeAddPinIcon"), nodeAddPinIcon);
	_styleSet->Set(TEXT("EasyQuestEditor.NodeDeletePinIcon"), nodeDeletePinIcon);
	_styleSet->Set(TEXT("EasyQuestEditor.NodeDeleteNodeIcon"), nodeDeleteNodeIcon);

	FSlateStyleRegistry::RegisterSlateStyle(*_styleSet);

	_pinFacotry = MakeShareable(new FEasyQuestPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(_pinFacotry);

	_nodeFactory = MakeShareable(new FEasyQuestNodeFactory());  
	FEdGraphUtilities::RegisterVisualNodeFactory(_nodeFactory);  
}

void FEasyQuestEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (_styleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*_styleSet);
		_styleSet.Reset();
	}
	FEdGraphUtilities::UnregisterVisualPinFactory(_pinFacotry);
	FEdGraphUtilities::UnregisterVisualNodeFactory(_nodeFactory);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasyQuestEditorModule, EasyQuestEditor)