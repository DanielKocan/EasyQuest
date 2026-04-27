#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Quest/QuestCriteriaTypes.h"
#include "Quest/BaseQuest.h"
#include "Quest/QuestTrackerWidget.h"
#include "QuestTriggerActor.generated.h"

class UEasyQuestAsset;

UCLASS(Blueprintable)
class WHISKEREDAWAY_API AQuestTriggerActor : public AActor
{
    GENERATED_BODY()

public:
    AQuestTriggerActor();

    UPROPERTY(EditAnywhere, Category = "Quest Trigger")
    bool bAutoComplete = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Trigger")
    TArray<FQuestCriteria> Tasks;

    UFUNCTION(BlueprintPure, Category = "Quest Trigger")
    bool AreAllTasksCompleted() const
    {
        for (const FQuestCriteria& Task : Tasks)
        {
            if (!Task.bIsCompleted) return false;
        }
        return Tasks.Num() > 0;  // false if no tasks defined
    }

    UFUNCTION(BlueprintCallable, Category = "Quest Trigger")
    void SetTaskCompleted(int32 TaskIndex, bool bCompleted)
    {
        if (Tasks.IsValidIndex(TaskIndex))
        {
            Tasks[TaskIndex].bIsCompleted = bCompleted;
        }
    }

    // Call from Blueprint when your completion condition is met
    UFUNCTION(BlueprintCallable, Category = "Quest Trigger")
    void TryCompleteQuest();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;  // For rotation

    // Reference to the quest asset (same one used by QuestManager)
    UPROPERTY(EditAnywhere, Category = "Quest Trigger")
    TSoftObjectPtr<UEasyQuestAsset> QuestAsset;

    // The name of the quest to watch for (gets populated from asset)
    UPROPERTY(EditAnywhere, Category = "Quest Trigger", meta = (GetOptions = "GetQuestNameOptions"))
    FString QuestToWatch;

    // 3D Widget showing quest name
    UPROPERTY(BlueprintReadWrite, Category = "Components")
    UWidgetComponent* QuestLabelWidget;

    // Blueprint events
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Events")
    void OnQuestCompleted(UBaseQuest* Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Events")
    void OnQuestEnabled(UBaseQuest* Quest);

    // Blueprint event fired when the quest becomes Active (so BP knows when to start checking)
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Events")
    void OnQuestBecameActive(UBaseQuest* Quest);

    UPROPERTY()
    class UQuestManager* QuestManager;

    // Get quest name for Blueprint/Widget
    UFUNCTION(BlueprintPure, Category = "Quest Trigger")
    FString GetWatchedQuestName() const { return QuestToWatch; }

    UPROPERTY()
    bool bCheckCompletionCondition = false;

    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Events")
    void OnQuestConditionCheckTick();  // fires every tick when bCheckCompletionCondition is true

    UFUNCTION(BlueprintCallable, Category = "Quest Trigger")
    void PushTrackerUpdate(const TArray<FQuestCriteria>& Entries, const FString& ComebackText = "");

#if WITH_EDITOR
    // Provide options for dropdown
    UFUNCTION()
    TArray<FString> GetQuestNameOptions() const;
#endif

private:
    UFUNCTION()
    void HandleQuestCompleted(UBaseQuest* Quest);

    UFUNCTION()
    void HandleQuestEnabled(UBaseQuest* Quest);

    UPROPERTY()
    UBaseQuest* WatchedQuestRef;
};