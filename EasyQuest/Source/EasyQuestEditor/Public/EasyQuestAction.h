#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 Everything that we want to see in editor we need to add action for it.
 Basically: Defines what happens when you double-click your asset (opens custom editor)
 **/
class EasyQuestAction : public FAssetTypeActions_Base{
public:
	EasyQuestAction(EAssetTypeCategories::Type category);

public: //FAssetTypeActions_Base interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;;
	virtual void OpenAssetEditor(const TArray<UObject*>& inObjects, TSharedPtr<class IToolkitHost> editWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;

private:

	EAssetTypeCategories::Type AssetCategory;
};
