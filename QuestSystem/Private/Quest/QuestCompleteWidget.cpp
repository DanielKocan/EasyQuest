#include "Quest/QuestCompleteWidget.h"
#include "Quest/BaseQuest.h"
#include "Components/TextBlock.h"

void UQuestCompleteWidget::ShowQuestComplete(UBaseQuest* CompletedQuest)
{
    if (!CompletedQuest) return;

    if (!IsInViewport())
    {
        AddToViewport(999);
    }

    OnQuestCompleteShown(CompletedQuest->GetQuestName());
}