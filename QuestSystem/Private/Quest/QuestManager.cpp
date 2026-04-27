// Fill out your copyright notice in the Description page of Project Settings.

#include "Quest/QuestManager.h"
#include "Quest/BaseQuest.h"
#include "Quest/QuestSettings.h" 
#include "Quest/QuestDebugOverlay.h"
#include "Quest/QuestCompleteWidget.h"
#include "Engine/World.h"
//For saving
#include "Quest/QuestSaveGame.h"
#include "GameData/GameDataTrackerSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Subsystem functions
void UQuestManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Initialize QuestManager"));

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UQuestManager::OnLevelChanged); // we need to make sure it saves the progress when player goes from the main map to menu map

	// Get settings from Project Settings
	const UQuestSettings* Settings = GetDefault<UQuestSettings>();
	if (Settings)
	{
		if (!Settings->MainQuestGraphAsset.IsNull())
		{
			// Load the asset
			UEasyQuestAsset* LoadedAsset = Cast<UEasyQuestAsset>(Settings->MainQuestGraphAsset.TryLoad());

			if (LoadedAsset)
			{
				UE_LOG(LogTemp, Log, TEXT("QuestManager: Loading quests from settings..."));
				LoadQuestsFromAsset(LoadedAsset);
				CheckPendingQuests();
				LoadProgress();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("QuestManager: Failed to load quest asset from path: %s"),
					*Settings->MainQuestGraphAsset.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("QuestManager: No quest asset set in Project Settings -> Quest System"));
		}
	}

	FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UQuestManager::OnFirstTick);
}

void UQuestManager::OnFirstTick(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameViewportClient* Viewport = GI->GetGameViewportClient();
	if (!Viewport) return;

	FWorldDelegates::OnWorldPostActorTick.RemoveAll(this); // Unsubscribe immediately since we only need this once

	// Overlay - only in non-shipping, only once
#if !UE_BUILD_SHIPPING
	if (!bOverlayStarted && GetDefault<UQuestSettings>()->bShowQuestDebugOverlay)
	{
		bOverlayStarted = true;
		QuestDebugOverlay = SNew(SQuestDebugOverlay);
		QuestDebugOverlay->Start(GI);
		UE_LOG(LogTemp, Log, TEXT("QuestManager: Debug overlay started."));
	}
#endif
}

void UQuestManager::Deinitialize()
{
	SaveProgress();
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FWorldDelegates::OnWorldPostActorTick.RemoveAll(this);

	if (QuestDebugOverlay.IsValid())
	{
		QuestDebugOverlay->Stop();
		QuestDebugOverlay.Reset();
	}

	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize QuestManager"));
}

void UQuestManager::OnLevelChanged(const FString& MapName)
{
	SaveProgress();
}

bool UQuestManager::ShouldCreateSubsystem(UObject* Outer) const
{
	return !GetClass()->HasAnyClassFlags(CLASS_Abstract);
}

// Quest creation function used from blueprints
UBaseQuest* UQuestManager::CreateQuest(const FString& Name, 
	 				 const FString& Dialogue, 
					 const TArray<UBaseQuest*>& Dependencies)
{
	UBaseQuest* Quest = NewObject<UBaseQuest>(this);
	Quest->Initialize(Name, Dialogue, Dependencies);
	Quest->CompleteEvent.AddDynamic(this, &UQuestManager::OnQuestCompleted);
	QuestPool.Add(Quest); // any quest gets added to the quest pool so that the manager knows about all possible quests

	return Quest;
}

void UQuestManager::ActivateQuest(UBaseQuest* Quest)
{
	if(!Quest) return;

	// Already active — nothing to do
	if (Quest->GetState() == EQuestState::Active) return;

	// Deactivate current quest first
	if (CurrentQuest && CurrentQuest->GetState() == EQuestState::Active)
	{
		CurrentQuest->Disable();  // sets it back to Available
	}

	Quest->Enable();
	if(Quest->GetState() == EQuestState::Active)
	{
		CurrentQuest = Quest;
	}
}

UBaseQuest* UQuestManager::GetActiveQuest()
{
	return CurrentQuest;
}

TArray<UBaseQuest*> UQuestManager::GetCompletedQuests()
{
	TArray<UBaseQuest*> CompletedQuests;
	
	for(const auto& Quest : QuestPool)
	{
		if(!Quest)
		{
			continue;
		}

		if(Quest->GetState() == EQuestState::Complete)
		{
			CompletedQuests.Add(Quest);
		}
	}

	return CompletedQuests; // return an array of the completed quests from the quest pool
}

TArray<UBaseQuest*> UQuestManager::GetAvailableQuests()
{
	TArray<UBaseQuest*> AvailableQuests;

	for(const auto& Quest : QuestPool)
	{
		if(!Quest)
		{
			UE_LOG(LogTemp, Warning, TEXT("No available quests found"));
			continue;
		}

		if(Quest->GetState() == EQuestState::Available || Quest->GetState() == EQuestState::Active)
		{
			if (Quest->IsVisible())  // only show visible quests on board
			{
				AvailableQuests.Add(Quest);
			}
		}
	}

	return AvailableQuests; // return an array of the pending quests from the quest pool
}

TArray<UBaseQuest*> UQuestManager::GetQuestPool() const
{
	return QuestPool;
}

// Called whenever a quest gets completed
void UQuestManager::CheckPendingQuests()
{
	// Update all quest states first
	for (const auto& Quest : QuestPool)
	{
		if (!Quest) continue;

		// If blocked, check if it should become available
		if (Quest->GetState() == EQuestState::Blocked && Quest->DependentQuestsFinished())
		{
			Quest->SetState(EQuestState::Available);
			UE_LOG(LogTemp, Log, TEXT("Quest '%s' is now available"), *Quest->GetQuestName());
		}
	}

	// Auto-activate first available quest if no active quest
	/*if (!CurrentQuest)
	{
		for (const auto& Quest : QuestPool)
		{
			if (!Quest) continue;

			if (Quest->GetState() == EQuestState::Available)
			{
				ActivateQuest(Quest);
				return;
			}
		}
	}*/
}

// Called whenever a quest gets completed
void UQuestManager::OnQuestCompleted(UBaseQuest* CompletedQuest)
{
	if(CurrentQuest == CompletedQuest)
	{
		CurrentQuest = nullptr;
	}

	// Hide and destroy tracker widget
	if (QuestTrackerWidget)
	{
		QuestTrackerWidget->OnTrackerRemoved();
		QuestTrackerWidget->RemoveFromParent();
		QuestTrackerWidget = nullptr;  // null it so next quest creates a fresh one
	}

	CheckPendingQuests();

	// Show UI that quest is completed
	const UQuestSettings* Settings = GetDefault<UQuestSettings>();
	if (Settings && !Settings->QuestCompleteWidgetClass.IsNull())
	{
		TSubclassOf<UQuestCompleteWidget> WidgetClass = Settings->QuestCompleteWidgetClass.LoadSynchronous();
		if (WidgetClass){
			UGameInstance* GI = GetGameInstance();
			APlayerController* PC = GI ? GI->GetFirstLocalPlayerController() : nullptr;
			if (PC){
				UQuestCompleteWidget* Widget = CreateWidget<UQuestCompleteWidget>(PC, WidgetClass);
				if (Widget){
					Widget->ShowQuestComplete(CompletedQuest);
				}
			}
		}
	}

	SaveProgress();
}

void UQuestManager::LoadQuestsFromAsset(UEasyQuestAsset* QuestAsset)
{
	if (!QuestAsset || !QuestAsset->Graph)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestManager: Invalid quest asset"));
		return;
	}

	TMap<UEasyQuestRuntimeNode*, UBaseQuest*> NodeToQuestMap;

	// First pass: Create all quests
	for (UEasyQuestRuntimeNode* Node : QuestAsset->Graph->Nodes)
	{
		if (Node->NodeType == EEasyQuestNodeType::DialogNode)
		{
			UEasyQuestNodeInfo* NodeInfo = Cast<UEasyQuestNodeInfo>(Node->NodeInfo);
			if (NodeInfo)
			{
				FString QuestName = NodeInfo->Title.IsEmpty() ?
					NodeInfo->QuestDescription.ToString() :
					NodeInfo->Title.ToString();

				FString QuestDescription = NodeInfo->QuestDescription.ToString();

				UBaseQuest* Quest = CreateQuest(
					QuestName,
					QuestDescription,
					TArray<UBaseQuest*>()
				);

				NodeToQuestMap.Add(Node, Quest);

				UE_LOG(LogTemp, Log, TEXT("  Created quest: %s"), *QuestName);
			}
		}
	}

	// Second pass: Set up dependencies based on connections
	for (auto& Pair : NodeToQuestMap)
	{
		UEasyQuestRuntimeNode* Node = Pair.Key;
		UBaseQuest* Quest = Pair.Value;
		UE_LOG(LogTemp, Log, TEXT("Checking dependencies for: %s"), *Quest->GetQuestName());

		for (UEasyQuestRuntimePin* OutputPin : Node->OutputPins)
		{
			if (!OutputPin) continue;

			// CHANGED: Connection -> Connections (array)
			for (UEasyQuestRuntimePin* ConnectedPin : OutputPin->Connections)
			{
				if (!ConnectedPin) continue;

				UEasyQuestRuntimeNode* ChildNode = ConnectedPin->Parent;
				UBaseQuest* ChildQuest = NodeToQuestMap.FindRef(ChildNode);
				if (ChildQuest)
				{
					ChildQuest->AddDependency(Quest);
					UE_LOG(LogTemp, Log, TEXT("  + Quest '%s' depends on '%s'"),
						*ChildQuest->GetQuestName(), *Quest->GetQuestName());
				}
			}
		}
	}

	// THIRD PASS: Update quest states based on dependencies
	for (auto& Pair : NodeToQuestMap)
	{
		UBaseQuest* Quest = Pair.Value;

		if (Quest->GetDependencies().Num() > 0)
		{
			// Has dependencies - check if they're all complete
			if (!Quest->DependentQuestsFinished())
			{
				Quest->SetState(EQuestState::Blocked);
				UE_LOG(LogTemp, Log, TEXT("  Quest '%s' set to Blocked"), *Quest->GetQuestName());
			}
			else
			{
				Quest->SetState(EQuestState::Available);
				UE_LOG(LogTemp, Log, TEXT("  Quest '%s' set to Available"), *Quest->GetQuestName());
			}
		}
		// If no dependencies, it stays Available (already set in Initialize)
	}



	UE_LOG(LogTemp, Log, TEXT("QuestManager: Loaded %d quests"), NodeToQuestMap.Num());
	UE_LOG(LogTemp, Warning, TEXT("=== QUEST LIST ==="));
	UE_LOG(LogTemp, Warning, TEXT("Total Quests: %d"), QuestPool.Num());

	for (UBaseQuest* Quest : QuestPool)
	{
		if (!Quest) continue;

		FString StateName;
		switch (Quest->GetState())
		{
		case EQuestState::Blocked: StateName = TEXT("Blocked"); break;
		case EQuestState::Available: StateName = TEXT("Available"); break;
		case EQuestState::Active: StateName = TEXT("Active"); break;
		case EQuestState::Complete: StateName = TEXT("Complete"); break;
		}

		UE_LOG(LogTemp, Warning, TEXT("\n Quest: %s [%s]"), *Quest->GetQuestName(), *StateName);

		// Print dependencies
		if (Quest->GetDependencies().Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Dependencies:"));
			for (UBaseQuest* Dep : Quest->GetDependencies())
			{
				if (Dep)
				{
					UE_LOG(LogTemp, Warning, TEXT("    - %s"), *Dep->GetQuestName());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  No dependencies"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("=================="));
}

UQuestTrackerWidget* UQuestManager::GetQuestTrackerWidget()
{
	// Create it if it doesn't exist yet
	if (!QuestTrackerWidget)
	{
		const UQuestSettings* Settings = GetDefault<UQuestSettings>();
		if (Settings && !Settings->QuestTrackerWidgetClass.IsNull())
		{
			TSubclassOf<UQuestTrackerWidget> WidgetClass =
				Settings->QuestTrackerWidgetClass.LoadSynchronous();
			if (WidgetClass)
			{
				UGameInstance* GI = GetGameInstance();
				if (GI)
				{
					QuestTrackerWidget = CreateWidget<UQuestTrackerWidget>(GI, WidgetClass);
					if (QuestTrackerWidget)
					{
						QuestTrackerWidget->AddToViewport();
					}
				}
			}
		}
	}
	return QuestTrackerWidget;
}

void UQuestManager::SaveProgress() { UQuestSaveGame::Save(this); }
void UQuestManager::LoadProgress() { UQuestSaveGame::Load(this); }
void UQuestManager::ResetProgress() { UQuestSaveGame::Reset(this); }

bool UQuestManager::HasSaveGame() const { return UQuestSaveGame::DoesSaveExist(); }

FDateTime UQuestManager::GetSaveDateTime() const
{
	UQuestSaveGame* Save = UQuestSaveGame::LoadSave();
	return Save ? Save->SavedAt : FDateTime();
}