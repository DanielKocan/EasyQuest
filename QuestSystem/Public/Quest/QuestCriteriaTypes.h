#pragma once

#include "CoreMinimal.h"
#include "QuestCriteriaTypes.generated.h"

USTRUCT(BlueprintType)
struct FQuestCriteria
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsCompleted = false;
};