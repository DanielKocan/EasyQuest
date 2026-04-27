#include "Quest/QuestBoardWidget.h"
#include "Quest/QuestEntryWidget.h"
#include "Quest/QuestManager.h"
#include "Quest/BaseQuest.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void UQuestBoardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("QuestBoardWidget: NativeConstruct called"));

    // Check if QuestListBox is bound
    if (!QuestListBox)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestBoardWidget: QuestListBox is NULL! Check BindWidget in Blueprint"));
        return;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestBoardWidget: QuestListBox found"));
    }

    // Check if widget class is set
    if (!QuestEntryWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestBoardWidget: QuestEntryWidgetClass is NULL! Set it in Blueprint"));
        return;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestBoardWidget: QuestEntryWidgetClass is set"));
    }

    // Get quest manager
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        QuestManager = GameInstance->GetSubsystem<UQuestManager>();
        if (QuestManager)
        {
            UE_LOG(LogTemp, Log, TEXT("QuestBoardWidget: QuestManager found"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("QuestBoardWidget: QuestManager is NULL!"));
        }
    }

    RefreshQuestList();
}

void UQuestBoardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void UQuestBoardWidget::RefreshQuestList()
{
    if (!QuestManager || !QuestListBox || !QuestEntryWidgetClass) return;

    // Get AVAILABLE quests (changed from GetPendingQuests)
    TArray<UBaseQuest*> AvailableQuests = QuestManager->GetAvailableQuests();

    // Clear and rebuild
    QuestListBox->ClearChildren();
    DisplayedQuests.Empty();

    for (UBaseQuest* Quest : AvailableQuests)
    {
        if (Quest && !DisplayedQuests.Contains(Quest->GetQuestIdentifier()))
        {
            UQuestEntryWidget* EntryWidget = CreateWidget<UQuestEntryWidget>(this, QuestEntryWidgetClass);
            if (EntryWidget)
            {
                EntryWidget->SetQuestData(Quest);
                EntryWidget->SetParentBoard(this);
                UHorizontalBoxSlot* HorizontalSlot = QuestListBox->AddChildToHorizontalBox(EntryWidget);
                if (HorizontalSlot)
                {
                    HorizontalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                    HorizontalSlot->SetHorizontalAlignment(HAlign_Center);
                }
                DisplayedQuests.Add(Quest->GetQuestIdentifier());
            }
        }
    }
}