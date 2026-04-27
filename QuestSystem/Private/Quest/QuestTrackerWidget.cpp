#include "Quest/QuestTrackerWidget.h"

void UQuestTrackerWidget::UpdateTracker(const FString& QuestName, const TArray<FQuestCriteria>& Entries, const FString& ComebackText)
{
    OnTrackerUpdated(QuestName, Entries, ComebackText);
}