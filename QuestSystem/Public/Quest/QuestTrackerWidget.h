#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/QuestCriteriaTypes.h"
#include "QuestTrackerWidget.generated.h"

UCLASS()
class WHISKEREDAWAY_API UQuestTrackerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Tracker")
    void OnTrackerRemoved();

    UFUNCTION(BlueprintCallable, Category = "Quest Tracker")
    void UpdateTracker(const FString& QuestName, const TArray<FQuestCriteria>& Entries, const FString& ComebackText);

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Tracker")
    void OnTrackerUpdated(const FString& QuestName, const TArray<FQuestCriteria>& Entries, const FString& ComebackText);
};