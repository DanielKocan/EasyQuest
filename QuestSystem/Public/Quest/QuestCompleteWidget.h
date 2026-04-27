#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestCompleteWidget.generated.h"

class UTextBlock;
class UBaseQuest;

UCLASS()
class WHISKEREDAWAY_API UQuestCompleteWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void ShowQuestComplete(UBaseQuest* CompletedQuest);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestCompleteText;

    // Override this in Blueprint to play animations, sounds, etc.
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Complete")
    void OnQuestCompleteShown(const FString& QuestName);
};