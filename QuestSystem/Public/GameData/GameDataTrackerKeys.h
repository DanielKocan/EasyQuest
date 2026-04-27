#pragma once

#include "CoreMinimal.h"

/**
 * This is the ONLY place you should define key name constants for use in C++.
 * Each FName here must exactly match a Row Name in your GameDataTrackerTable DataTable.
 *
 * HOW TO ADD A NEW KEY:
 *   1. Add a row to your GameDataTrackerTable DataTable in the editor (Row Name = the key string)
 *   2. Add the constant below
 *   3. Add it to AllKeys
 *   4. The Blueprint dropdown updates automatically � no other changes needed
 *
 * USAGE EXAMPLE:
 *
 *  #include "GameData/GameDataTrackerSubsystem.h"
 *
 *  UGameDataTrackerSubsystem* Tracker = GetGameInstance()->GetSubsystem<UGameDataTrackerSubsystem>(); // You can add safety check later in new line: if (!Tracker) return;
 *
 *  Tracker->RecordInt(GameDataTrackerKeys::CatCount, 5); // to set
 *
 *  int32 CatCount = 0;
 *  Tracker->GetInt(GameDataTrackerKeys::CatCount, CatCount); // to get
 */
namespace GameDataTrackerKeys
{
    const FName CatCount =          "CatCount";            // int32
    const FName CaughtCatRef =      "CaughtCatRef";        // CatRef
    const FName QuestsCompleted =   "QuestsCompleted";     // int32
    const FName CollectibleCount =  "CollectibleCount";    // int32
    const FName PlayerPosition =    "PlayerPos";           // FVector
    
    // -------------------------------------------------------------------------
    // All keys - used for startup validation. Keep this in sync with the constants above!!! used only for the startup validation report
    // -------------------------------------------------------------------------
    inline const TArray<FName> AllKeys =
    {
        CatCount,
        CaughtCatRef,
        QuestsCompleted,
        CollectibleCount,
        PlayerPosition,
    };
}