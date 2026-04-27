#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestBoardWidget.h"
#include "QuestEntryWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;
class UBaseQuest;

UENUM(BlueprintType)
enum class EQuestButtonState : uint8
{
    Available        UMETA(DisplayName = "Available"),
    Active           UMETA(DisplayName = "Active"),
    ReadyToComplete  UMETA(DisplayName = "Ready To Complete"),
};

UCLASS()
class WHISKEREDAWAY_API UQuestEntryWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    // UI Elements
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestDescriptionText;

    UPROPERTY(meta = (BindWidget))
    UButton* StartQuestButton;

    UPROPERTY()
    UBaseQuest* Quest;

    UPROPERTY(meta = (BindWidget))
    UButton* ForceCompleteButton;

    UPROPERTY(meta = (BindWidget))
    UImage* QuestBackgroundImage;

    UPROPERTY(EditDefaultsOnly) // set once in blueprint only
    UTexture2D* ActiveQuestBackgroundTexture;
     
    UPROPERTY(EditDefaultsOnly)  // set once in blueprint only
    UTexture2D * AvailableQuestBackgroundTexture;

    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Entry")
    void OnQuestStateChanged(EQuestButtonState ButtonState);

public:
    void SetQuestData(UBaseQuest* InQuest);

    void SetParentBoard(UQuestBoardWidget* InParentBoard) { ParentBoard = InParentBoard; }

private:
    UFUNCTION()
    void OnStartQuestClicked();

    UPROPERTY()
    UQuestBoardWidget* ParentBoard;

    UFUNCTION()
    void OnForceCompleteClicked();


    UFUNCTION()
    void UpdateButtonState();
};