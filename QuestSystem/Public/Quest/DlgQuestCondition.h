#pragma once
#include "CoreMinimal.h"
#include "DlgSystem/DlgConditionCustom.h"
#include "EasyQuestAsset.h"
#include "EasyQuestRuntimeGraph.h"
#include "EasyQuestNodeInfo.h"
#include "DlgQuestCondition.generated.h"

UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "Quest Condition"))
class WHISKEREDAWAY_API UDlgQuestCondition : public UDlgConditionCustom
{
    GENERATED_BODY()

public:
    bool IsConditionMet_Implementation(const UDlgContext* Context, const UObject* Participant) override;

#if WITH_EDITOR
    FString GetEditorDisplayString_Implementation(UDlgDialogue* OwnerDialogue, FName ParticipantName) override
    {
        return FString::Printf(TEXT("Quest '%s' is %s"), *QuestToCheck, bShouldBeCompleted ? TEXT("Complete") : TEXT("Not Complete"));
    }

    UFUNCTION()
    TArray<FString> GetQuestNameOptions() const;
#endif

protected:
    UPROPERTY(EditAnywhere, Category = "Quest")
    TSoftObjectPtr<UEasyQuestAsset> QuestAsset;

    UPROPERTY(EditAnywhere, Category = "Quest", meta = (GetOptions = "GetQuestNameOptions"))
    FString QuestToCheck;

    // true = check if complete, false = check if NOT complete
    UPROPERTY(EditAnywhere, Category = "Quest")
    bool bShouldBeCompleted = true;
};
