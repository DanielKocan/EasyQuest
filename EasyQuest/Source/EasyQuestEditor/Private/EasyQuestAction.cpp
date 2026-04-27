#include "EasyQuestAction.h"
#include "EasyQuestAsset.h"
#include "EasyQuestEditorApp.h"

EasyQuestAction::EasyQuestAction(EAssetTypeCategories::Type category)
{
	AssetCategory = category;
}

FText EasyQuestAction::GetName() const
{
	return FText::FromString(TEXT("Easy Quest Asset"));
}

FColor EasyQuestAction::GetTypeColor() const
{
	return FColor::Cyan;
}

UClass* EasyQuestAction::GetSupportedClass() const
{
	return UEasyQuestAsset::StaticClass();
}

void EasyQuestAction::OpenAssetEditor(const TArray<UObject*>& inObjects, TSharedPtr<class IToolkitHost> editWithinLevelEditor)
{
	EToolkitMode::Type mode = editWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (UObject* object : inObjects)
	{
		UEasyQuestAsset* customAssset = Cast<UEasyQuestAsset>(object);
		if (customAssset != nullptr)
		{
			TSharedRef<EasyQuestEditorApp> editor(new EasyQuestEditorApp);
			editor->InitEditor(mode, editWithinLevelEditor, customAssset);
		}
	}
}

uint32 EasyQuestAction::GetCategories()
{
	return AssetCategory;
}





