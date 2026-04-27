#include "Quest/QuestTriggerActor.h"
#include "Quest/QuestManager.h"
#include "EasyQuestAsset.h"
#include "EasyQuestRuntimeGraph.h"
#include "EasyQuestNodeInfo.h"
#include "Kismet/GameplayStatics.h"

AQuestTriggerActor::AQuestTriggerActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root scene component
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // Create 3D widget label
    QuestLabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("QuestLabel"));
    QuestLabelWidget->SetupAttachment(RootComponent);
    QuestLabelWidget->SetWidgetSpace(EWidgetSpace::World);
    QuestLabelWidget->SetDrawSize(FVector2D(800, 800));
    QuestLabelWidget->SetRelativeLocation(FVector(0, 0, 50));
}

void AQuestTriggerActor::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GameInstance = GetWorld()->GetGameInstance();
    if (GameInstance)
    {
        QuestManager = GameInstance->GetSubsystem<UQuestManager>();

        if (QuestManager)
        {
            for (UBaseQuest* Quest : QuestManager->GetQuestPool())
            {
                if (Quest && Quest->GetQuestName() == QuestToWatch)
                {
                    WatchedQuestRef = Quest;
                    Quest->EnableEvent.AddDynamic(this, &AQuestTriggerActor::HandleQuestEnabled);
                    Quest->CompleteEvent.AddDynamic(this, &AQuestTriggerActor::HandleQuestCompleted);

                    UE_LOG(LogTemp, Log, TEXT("QuestTrigger: Watching quest '%s'"), *QuestToWatch);

                    // If quest was already active when we loaded, fire manually
                    if (WatchedQuestRef->GetState() == EQuestState::Active)
                    {
                        FTimerHandle TempHandle;
                        GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]() // we need this timer because we have to wait after all widget will be created
                            {
                                UE_LOG(LogTemp, Log, TEXT("QuestTrigger: Quest '%s' restored as active from save"), *QuestToWatch);
                                HandleQuestEnabled(WatchedQuestRef);
                            }, 0.1f, false);
                    }

                    return;
                }
            }

            UE_LOG(LogTemp, Warning, TEXT("QuestTrigger: Quest '%s' not found!"), *QuestToWatch);
        }
    }
}

void AQuestTriggerActor::Tick(float DeltaTime)
{
    // Rotate widget to face camera
    if (QuestLabelWidget)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC && PC->PlayerCameraManager)
        {
            FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
            FVector WidgetLocation = QuestLabelWidget->GetComponentLocation();

            FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();
            QuestLabelWidget->SetWorldRotation(LookAtRotation);
        }
    }

    // Fire Blueprint event every tick while checking
    if (bCheckCompletionCondition && WatchedQuestRef)
    {
        OnQuestConditionCheckTick();

        if (AreAllTasksCompleted())
        {
            WatchedQuestRef->SetAllCriteriaMet(true);  // set the flag on the quest

            if (bAutoComplete)
            {
                TryCompleteQuest();
                bCheckCompletionCondition = false;
            }
        }
        else
        {
            WatchedQuestRef->SetAllCriteriaMet(false);  // reset if criteria become unmet
        }
    }
}

#if WITH_EDITOR
TArray<FString> AQuestTriggerActor::GetQuestNameOptions() const
{
    TArray<FString> Options;

    // Load the quest asset
    if (!QuestAsset.IsNull())
    {
        UEasyQuestAsset* LoadedAsset = QuestAsset.LoadSynchronous();
        if (LoadedAsset && LoadedAsset->Graph)
        {
            // Extract quest names from nodes
            for (UEasyQuestRuntimeNode* Node : LoadedAsset->Graph->Nodes)
            {
                if (Node->NodeType == EEasyQuestNodeType::DialogNode)
                {
                    UEasyQuestNodeInfo* NodeInfo = Cast<UEasyQuestNodeInfo>(Node->NodeInfo);
                    if (NodeInfo)
                    {
                        FString QuestName = NodeInfo->Title.IsEmpty() ?
                            NodeInfo->QuestDescription.ToString() :
                            NodeInfo->Title.ToString();

                        Options.Add(QuestName);
                    }
                }
            }
        }
    }

    if (Options.Num() == 0)
    {
        Options.Add(TEXT("(Set Quest Asset first)"));
    }

    return Options;
}
#endif

void AQuestTriggerActor::TryCompleteQuest()
{
    if (!WatchedQuestRef) return;

    if (WatchedQuestRef->GetState() != EQuestState::Active)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestTrigger: Quest '%s' is not Active"), *QuestToWatch);
        return;
    }

    if (!WatchedQuestRef->IsAllCriteriaMet())
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestTrigger: Criteria not met yet"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("QuestTrigger: Completing quest '%s'"), *QuestToWatch);
    WatchedQuestRef->Complete();

    if (QuestManager)
    {
        QuestManager->CheckPendingQuests();
    }
}

void AQuestTriggerActor::HandleQuestEnabled(UBaseQuest* Quest)
{
    bCheckCompletionCondition = true;
    if (QuestManager) QuestManager->SetActiveTriggerActor(this);
    OnQuestEnabled(Quest);  
    OnQuestBecameActive(Quest);
}

void AQuestTriggerActor::HandleQuestCompleted(UBaseQuest* Quest)
{
    bCheckCompletionCondition = false;
    if (QuestManager) QuestManager->SetActiveTriggerActor(nullptr);
    OnQuestCompleted(Quest);
}

void AQuestTriggerActor::PushTrackerUpdate(const TArray<FQuestCriteria>& Entries, const FString& ComebackText)
{
    if (QuestManager && WatchedQuestRef)
    {
        UQuestTrackerWidget* Tracker = QuestManager->GetQuestTrackerWidget();
        if (Tracker)
        {
            Tracker->UpdateTracker(WatchedQuestRef->GetQuestName(), Entries, ComebackText);
        }
    }
}