#include "Quest/QuestSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameData/GameDataTrackerSubsystem.h"
#include "Quest/QuestManager.h"

const FString UQuestSaveGame::SaveSlotName = TEXT("QuestSave");
const int32 UQuestSaveGame::UserIndex = 0;

void UQuestSaveGame::Save(UQuestManager* Manager)
{
    // Don't save if nothing is loaded — prevents menu level from overwriting good data
    if (!Manager || Manager->GetQuestPool().Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestSaveGame: Skipping save - nothing loaded"));
        return;
    }

    UQuestSaveGame* SaveData = Cast<UQuestSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UQuestSaveGame::StaticClass()));
    if (!SaveData) return;

    SaveData->SavedAt = FDateTime::Now();

    for (UBaseQuest* Quest : Manager->GetQuestPool())
    {
        if (!Quest) continue;
        FQuestSaveEntry Entry;
        Entry.QuestName = Quest->GetQuestName();
        Entry.State = Quest->GetState();
        Entry.bVisible = Quest->IsVisible();
        SaveData->QuestStates.Add(Entry);
    }

    UBaseQuest* ActiveQuest = Manager->GetActiveQuest();
    SaveData->ActiveQuestName = ActiveQuest ? ActiveQuest->GetQuestName() : TEXT("");

    UGameInstance* GI = Manager->GetGameInstance();
    UE_LOG(LogTemp, Log, TEXT("QuestSaveGame Save: GI = %s"), GI ? TEXT("VALID") : TEXT("NULL"));
    UGameDataTrackerSubsystem* Tracker = GI ? GI->GetSubsystem<UGameDataTrackerSubsystem>() : nullptr;
    UE_LOG(LogTemp, Log, TEXT("QuestSaveGame Save: Tracker = %s"), Tracker ? TEXT("VALID") : TEXT("NULL"));

    if (Tracker)
    {
        SaveData->TrackerData = Tracker->GetAllData();

        for (const auto& Pair : SaveData->TrackerData)
        {
            UE_LOG(LogTemp, Log, TEXT("QuestSaveGame: Saving key '%s'"), *Pair.Key.ToString());
            if (Pair.Value.DataType == ETrackedDataType::Integer)
                UE_LOG(LogTemp, Log, TEXT("  Value: %d"), Pair.Value.IntValue);
        }
    }

    SaveData->DlgHistory = FDlgMemory::Get().GetHistoryMaps();

    UQuestSaveGame* ExistingSave = LoadSave(); 

    // --- Save Collectibles ---

    if (ExistingSave)
    {
        SaveData->CollectedCollectibleIDs = ExistingSave->CollectedCollectibleIDs;
    }

    UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, UserIndex);
    UE_LOG(LogTemp, Log, TEXT("QuestSaveGame: Saved. DlgHistory entries: %d"), SaveData->DlgHistory.Num());
}

void UQuestSaveGame::Load(UQuestManager* Manager)
{
    if (!Manager) return;

    if (!DoesSaveExist())
    {
        UE_LOG(LogTemp, Log, TEXT("QuestSaveGame: No save found, resetting."));
        Manager->ResetProgress(); // clear any leftover history from previous session!
        return;
    }

    UQuestSaveGame* SaveData = LoadSave();
    if (!SaveData) return;

    for (const FQuestSaveEntry& Entry : SaveData->QuestStates)
    {
        for (UBaseQuest* Quest : Manager->GetQuestPool())
        {
            if (Quest && Quest->GetQuestName() == Entry.QuestName)
            {
                Quest->SetState(Entry.State);
                Quest->SetVisible(Entry.bVisible);
                if (Entry.State == EQuestState::Active)
                {
                    // use setter if you have one, or keep this pattern
                    Quest->EnableEvent.Broadcast(Quest);
                }
                break;
            }
        }
    }

    // Restore tracker data
    UGameInstance* GI = Manager->GetGameInstance();
    UGameDataTrackerSubsystem* Tracker = GI ? GI->GetSubsystem<UGameDataTrackerSubsystem>() : nullptr;
    if (Tracker)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestSaveGame: Saving %d tracker entries"), SaveData->TrackerData.Num());
        for (const auto& Pair : SaveData->TrackerData)
        {
            switch (Pair.Value.DataType)
            {
            case ETrackedDataType::Integer: Tracker->RecordInt(Pair.Key, Pair.Value.IntValue); break;
            case ETrackedDataType::Float:   Tracker->RecordFloat(Pair.Key, Pair.Value.FloatValue); break;
            case ETrackedDataType::Bool:    Tracker->RecordBool(Pair.Key, Pair.Value.BoolValue);   break;
            case ETrackedDataType::String:  Tracker->RecordString(Pair.Key, Pair.Value.StringValue); break;
            case ETrackedDataType::Vector:  Tracker->RecordVector(Pair.Key, Pair.Value.VectorValue); break;
            default: break;
            }
        }
    }

    if (SaveData->DlgHistory.Num() > 0)
        FDlgMemory::Get().SetHistoryMap(SaveData->DlgHistory);

    UE_LOG(LogTemp, Log, TEXT("QuestSaveGame: Loaded. DlgHistory entries: %d"), SaveData->DlgHistory.Num());
}

void UQuestSaveGame::Reset(UQuestManager* Manager)
{
    Delete();
    Manager->SetCurrentQuest(nullptr);

    for (UBaseQuest* Quest : Manager->GetQuestPool())
    {
        if (!Quest) continue;
        Quest->SetState(Quest->GetDependencies().Num() == 0 ? EQuestState::Available : EQuestState::Blocked);
    }

    UGameInstance* GI = Manager->GetGameInstance();
    UGameDataTrackerSubsystem* Tracker = GI ? GI->GetSubsystem<UGameDataTrackerSubsystem>() : nullptr;
    if (Tracker) Tracker->ResetAllData();

    FDlgMemory::Get().Empty();
    UE_LOG(LogTemp, Log, TEXT("QuestSaveGame: Progress reset."));
}

void UQuestSaveGame::Delete()
{
    if (DoesSaveExist())
        UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);
}

bool UQuestSaveGame::DoesSaveExist()
{
    return UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex);
}

UQuestSaveGame* UQuestSaveGame::LoadSave()
{
    if (!DoesSaveExist()) return nullptr;
    return Cast<UQuestSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
}

void UQuestSaveGame::MarkCollectibleCollected(FName CollectibleID)
{
    UQuestSaveGame* Save = LoadSave();
    if (!Save) Save = Cast<UQuestSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UQuestSaveGame::StaticClass()));
    if (!Save) return;

    Save->CollectedCollectibleIDs.AddUnique(CollectibleID);
    UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, UserIndex);
}

bool UQuestSaveGame::IsCollectibleCollected(FName CollectibleID)
{
    UQuestSaveGame* Save = LoadSave();
    if (!Save) return false;
    return Save->CollectedCollectibleIDs.Contains(CollectibleID);
}