#pragma once

#include "CoreMinimal.h"
#include "EasyQuestRuntimeGraph.h"
#include <functional>
#include "EasyQuestAsset.generated.h"

// Actual quest data asset (what gets saved in the Content Browser)
// Saves happens here!
UCLASS(BlueprintType)
class EASYQUESTRUNTIME_API UEasyQuestAsset : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY()
	UEasyQuestRuntimeGraph* Graph = nullptr;

public: // our interface
	void SetPreSaveListener(std::function<void()> onPreSaveListener) { _onPreSaveListener = onPreSaveListener; }

public: // UObject interface
	virtual void PreSave(FObjectPreSaveContext saveContext) override;

private: // Members
	std::function<void()> _onPreSaveListener = nullptr;
};
