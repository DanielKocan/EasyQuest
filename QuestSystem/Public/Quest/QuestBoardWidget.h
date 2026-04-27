#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestBoardWidget.generated.h"

class UHorizontalBox;
class UQuestEntryWidget;
class UBaseQuest;

UCLASS()
class WHISKEREDAWAY_API UQuestBoardWidget : public UUserWidget
{
    GENERATED_BODY()

public: 
    void RefreshQuestList();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Container for quest entries
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* QuestListBox;

    // Widget class for individual quest entries
    UPROPERTY(EditDefaultsOnly, Category = "Quest Board")
    TSubclassOf<UQuestEntryWidget> QuestEntryWidgetClass;

private:

    UPROPERTY()
    class UQuestManager* QuestManager;

    // Track which quests we've displayed
    TSet<FGuid> DisplayedQuests;
};