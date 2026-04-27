#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameDataTrackerSettings.generated.h"

// Allows us to see setting in project settings
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Data Tracker"))
class WHISKEREDAWAY_API UGameDataTrackerSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    // This shows up in Project Settings -> Game -> Game Data Tracker
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameDataTracker",
        meta = (DisplayName = "Field Definition Table"))
    TSoftObjectPtr<UDataTable> DefinitionTable;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug",
        meta = (DisplayName = "Show Debug Overlay"))
    bool bShowDebugOverlay = false;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug",
        meta = (DisplayName = "Debug Overlay Update Interval (seconds)"))
    float DebugOverlayUpdateInterval = 0.5f;

    // UDeveloperSettings interface — puts it under the "Game" section
    virtual FName GetCategoryName() const override { return FName("Game"); }
};
