#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Quest/BaseQuest.h"
#include "GameData/GameDataTrackerTypes.h"
#include "DlgSystem/DlgMemory.h"
#include "QuestSaveGame.generated.h"

USTRUCT()
struct FQuestSaveEntry
{
    GENERATED_BODY()

    UPROPERTY()
    FString QuestName;

    UPROPERTY()
    EQuestState State = EQuestState::Blocked;

    UPROPERTY()
    bool bVisible = false; 
};

class UQuestManager;

UCLASS()
class WHISKEREDAWAY_API UQuestSaveGame : public USaveGame
{
    GENERATED_BODY()

public:

    static void Save(UQuestManager* Manager);
    static void Load(UQuestManager* Manager);
    static void Reset(UQuestManager* Manager);
    static void Delete();
    static bool DoesSaveExist();
    static UQuestSaveGame* LoadSave();

public:
    UPROPERTY()
    TArray<FQuestSaveEntry> QuestStates;

    UPROPERTY()
    FDateTime SavedAt;

    UPROPERTY()
    FString ActiveQuestName;

    UPROPERTY()
    TMap<FName, FGameDataFieldValue> TrackerData;

    UPROPERTY()
    TMap<FGuid, FDlgHistory> DlgHistory;

    static const FString SaveSlotName;
    static const int32 UserIndex;

public: // Collectibles

    UPROPERTY()
    TArray<FName> CollectedCollectibleIDs;

    // Static helpers for collectibles specifically
    static void MarkCollectibleCollected(FName CollectibleID);
    static bool IsCollectibleCollected(FName CollectibleID);
};