

#pragma once

#include "CoreMinimal.h"
#include "EasyQuestNodeInfoBase.h"
#include "EasyQuestEndNodeInfo.generated.h"

UENUM(BlueprintType)
enum class EEasyQuestNodeAction : uint8 {
	None,
	StartQuest // ActionData is the QuestId
};

UCLASS()
class UEasyQuestEndNodeInfo : public UEasyQuestNodeInfoBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EEasyQuestNodeAction Action = EEasyQuestNodeAction::None;

	UPROPERTY(EditAnywhere)
	FString ActionData = TEXT("");
};
