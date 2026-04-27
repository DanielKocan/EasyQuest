#include "Quest/QuestEntryWidget.h"
#include "Quest/BaseQuest.h"
#include "Quest/QuestManager.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Slate/SlateBrushAsset.h"

void UQuestEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("QuestEntryWidget: NativeConstruct called"));

    // Check bindings
    if (!QuestNameText)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestEntryWidget: QuestNameText is NULL! Check BindWidget"));
    }

    if (!QuestDescriptionText)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestEntryWidget: QuestDescriptionText is NULL! Check BindWidget"));
    }

    if (!StartQuestButton)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestEntryWidget: StartQuestButton is NULL! Check BindWidget"));
    }
    else
    {
        StartQuestButton->OnClicked.AddDynamic(this, &UQuestEntryWidget::OnStartQuestClicked);
        UE_LOG(LogTemp, Log, TEXT("QuestEntryWidget: Button bound successfully"));
    }

    if (ForceCompleteButton)
    {
        ForceCompleteButton->OnClicked.AddDynamic(this, &UQuestEntryWidget::OnForceCompleteClicked);
    }
}

void UQuestEntryWidget::SetQuestData(UBaseQuest* InQuest)
{
    Quest = InQuest;

    UE_LOG(LogTemp, Warning, TEXT("QuestEntryWidget: SetQuestData called for quest: %s"),
        Quest ? *Quest->GetQuestName() : TEXT("NULL"));

    if (Quest)
    {
        if (QuestNameText)
        {
            QuestNameText->SetText(FText::FromString(Quest->GetQuestName()));
            UE_LOG(LogTemp, Log, TEXT("  Set quest name text"));
        }

        if (QuestDescriptionText)
        {
            QuestDescriptionText->SetText(FText::FromString(Quest->GetQuestDescription()));
            UE_LOG(LogTemp, Log, TEXT("  Set quest description text"));
        }
    }

    UpdateButtonState();
}

void UQuestEntryWidget::OnStartQuestClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("QuestEntryWidget: Start button clicked!"));

    if (!Quest) return;

    UGameInstance* GI = GetGameInstance();
    UQuestManager* QM = GI ? GI->GetSubsystem<UQuestManager>() : nullptr;
    if (!QM) return;

    if (Quest->GetState() == EQuestState::Available)
    {
        // Block activation if a DIFFERENT quest is already active
        UBaseQuest* ActiveQuest = QM->GetActiveQuest();
        if (ActiveQuest && ActiveQuest != Quest)
        {
            UE_LOG(LogTemp, Warning, TEXT("QuestEntryWidget: Cannot start '%s' - quest '%s' is already active"),
                *Quest->GetQuestName(), *ActiveQuest->GetQuestName());
            return;  // Do nothing — player must finish current quest first
        }

        QM->ActivateQuest(Quest);
        QM->CheckPendingQuests();
    }
    else if (Quest->GetState() == EQuestState::Active && Quest->IsAllCriteriaMet())
    {
        AQuestTriggerActor* Trigger = QM->GetActiveTriggerActor();
        if (Trigger) Trigger->TryCompleteQuest();
    }

    if (ParentBoard) ParentBoard->RefreshQuestList();
    UpdateButtonState();
}

void UQuestEntryWidget::OnForceCompleteClicked()
{
    if (Quest && Quest->GetState() == EQuestState::Active)
    {
        UGameInstance* GameInstance = GetGameInstance();
        if (GameInstance)
        {
            UQuestManager* QuestManager = GameInstance->GetSubsystem<UQuestManager>();
            if (QuestManager)
            {
                Quest->Complete();
                QuestManager->CheckPendingQuests();

                if (ParentBoard)
                {
                    ParentBoard->RefreshQuestList();
                }
            }
        }
    }
}

void UQuestEntryWidget::UpdateButtonState()
{
    if (!Quest) return;

    if (Quest->GetState() == EQuestState::Active)
    {
        // Only show ReadyToComplete button if NOT auto completing
        UGameInstance* GI = GetGameInstance();
        UQuestManager* QM = GI ? GI->GetSubsystem<UQuestManager>() : nullptr;
        AQuestTriggerActor* Trigger = QM ? QM->GetActiveTriggerActor() : nullptr;

        if (Trigger && !Trigger->bAutoComplete && Quest->IsAllCriteriaMet())
        {
            OnQuestStateChanged(EQuestButtonState::ReadyToComplete);
        }
        else
        {
            OnQuestStateChanged(EQuestButtonState::Active);
            QuestBackgroundImage->SetBrushFromTexture(ActiveQuestBackgroundTexture);
        }
    }
    else
    {
        OnQuestStateChanged(EQuestButtonState::Available);
        QuestBackgroundImage->SetBrushFromTexture(AvailableQuestBackgroundTexture);
    }
}