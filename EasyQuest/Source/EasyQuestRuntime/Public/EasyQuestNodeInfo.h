#pragma once

#include "CoreMinimal.h"
#include "EasyQuestNodeInfoBase.h"
#include "EasyQuestNodeInfo.generated.h"

UCLASS(BlueprintType)
class EASYQUESTRUNTIME_API UEasyQuestNodeInfo : public UEasyQuestNodeInfoBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FText Title;

    UPROPERTY(EditAnywhere)
    FText QuestDescription;

    UPROPERTY(EditAnywhere)
    bool isCompleted;

    UPROPERTY(EditAnywhere)
    TArray<FText> DialogResponses;
};
