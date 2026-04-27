#include "Quest/DlgQuestNode.h"
#include "Quest/QuestManager.h"
#include "Quest/BaseQuest.h"
#include "DlgSystem/DlgContext.h"
#include "Kismet/GameplayStatics.h"

bool UDlgQuestNode::HandleNodeEnter(UDlgContext& Context, TSet<const UDlgNode*> NodesEnteredWithThisStep)
{
    FireNodeEnterEvents(Context);

    // Guard against endless loops (taken from selector node class from plugin)
    if (NodesEnteredWithThisStep.Contains(this))
    {
        return false;
    }
    NodesEnteredWithThisStep.Add(this);

    // Do quest reveal& activaiton logic
    UGameInstance* GI = UGameplayStatics::GetGameInstance(&Context);
    if (GI)
    {
        UQuestManager* QM = GI->GetSubsystem<UQuestManager>();
        if (QM)
        {
            for (UBaseQuest* Quest : QM->GetQuestPool())
            {
                if (Quest && Quest->GetQuestName() == QuestToReveal)
                {
                    Quest->SetVisible(true);
                    if (bActivateQuest)
                    {
                        QM->ActivateQuest(Quest);
                    }
                    break;
                }
            }
        }
    }

    // Auto-advance exactly like Selector (First) does
    for (const FDlgEdge& Edge : Children)
    {
        if (Edge.Evaluate(Context, { this }))
        {
            return Context.EnterNode(Edge.TargetIndex, NodesEnteredWithThisStep);
        }
    }

    // No satisfied child - terminate
    return false;
}