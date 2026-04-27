#pragma once
#include "CoreMinimal.h"
#include "DlgSystem/Nodes/DlgNode_Custom.h"
#include "EasyQuestAsset.h"
#include "EasyQuestRuntimeGraph.h"
#include "EasyQuestNodeInfo.h"
#include "DlgQuestNode.generated.h"

UCLASS(BlueprintType, ClassGroup = "Dialogue")
class WHISKEREDAWAY_API UDlgQuestNode : public UDlgNode_Custom
{
    GENERATED_BODY()

public:
    bool HandleNodeEnter(UDlgContext& Context, TSet<const UDlgNode*> NodesEnteredWithThisStep) override;

#if WITH_EDITOR
    FString GetNodeTypeString() const override { return TEXT("Quest Reveal"); }
    FLinearColor GetNodeColor() const override { return FLinearColor(0.2f, 0.8f, 0.2f, 1.f); }

    bool GetNodeTitleOverride(FString& OutTitle) const override
    {
        if (!QuestToReveal.IsEmpty())
        {
            OutTitle = TEXT("Reveal: ") + QuestToReveal;
            return true;  // use title instead of participant name
        }
        return false;  // fall back to default title
    }
#endif

    UPROPERTY(EditAnywhere, Category = "Quest")
    bool bActivateQuest = true;

protected:
    UPROPERTY(EditAnywhere, Category = "Quest")
    TSoftObjectPtr<UEasyQuestAsset> QuestAsset;

    UPROPERTY(EditAnywhere, Category = "Quest", meta = (GetOptions = "GetQuestNameOptions"))
    FString QuestToReveal;

#if WITH_EDITOR
    UFUNCTION()
    TArray<FString> GetQuestNameOptions() const //tbh its better to make a function later and use in multiple files TODO
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

        if (Options.Num() == 0)
        {
            Options.Add(TEXT("(Set Quest Asset first)"));
        }

        return Options;
    }
#endif
};