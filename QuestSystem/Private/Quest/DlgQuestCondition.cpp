#include "Quest/DlgQuestCondition.h"
#include "Quest/QuestManager.h"
#include "Quest/BaseQuest.h"
#include "DlgSystem/DlgContext.h"
#include "Kismet/GameplayStatics.h"

bool UDlgQuestCondition::IsConditionMet_Implementation(const UDlgContext* Context, const UObject* Participant)
{
    if (!Context) return false;

    UGameInstance* GI = UGameplayStatics::GetGameInstance(Context);
    if (!GI) return false;

    UQuestManager* QM = GI->GetSubsystem<UQuestManager>();
    if (!QM) return false;

    for (UBaseQuest* Quest : QM->GetQuestPool())
    {
        if (Quest && Quest->GetQuestName() == QuestToCheck)
        {
            bool bIsComplete = Quest->GetState() == EQuestState::Complete;
            return bIsComplete == bShouldBeCompleted;
        }
    }

    return false;
}

#if WITH_EDITOR
TArray<FString> UDlgQuestCondition::GetQuestNameOptions() const
{
    TArray<FString> Options;

    if (!QuestAsset.IsNull())
    {
        UEasyQuestAsset* LoadedAsset = QuestAsset.LoadSynchronous();
        if (LoadedAsset && LoadedAsset->Graph)
        {
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

    if (Options.Num() == 0) Options.Add(TEXT("(Set Quest Asset first)"));
    return Options;
}
#endif