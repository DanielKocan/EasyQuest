#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "QuestSettings.generated.h"

class UEasyQuestAsset;

/**
  Quest System Settings - just to appear in Project Settings
**/
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Quest System"))
class WHISKEREDAWAY_API UQuestSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UQuestSettings()
    {
        CategoryName = TEXT("Game");
        SectionName = TEXT("Quest System");
    }

    // The main quest graph asset to load
    UPROPERTY(config, EditAnywhere, Category = "Quest Configuration")
    FSoftObjectPath MainQuestGraphAsset;

    /** Show quest debug overlay in top-left corner during Play */
    UPROPERTY(config, EditAnywhere, Category = "Debug",
        meta = (DisplayName = "Show Quest Debug Overlay"))
    bool bShowQuestDebugOverlay = false;

    UPROPERTY(config, EditAnywhere, Category = "Quest Configuration")
    TSoftClassPtr<UUserWidget> QuestCompleteWidgetClass;

    UPROPERTY(config, EditAnywhere, Category = "Quest Configuration")
    TSoftClassPtr<UUserWidget> QuestTrackerWidgetClass;
};