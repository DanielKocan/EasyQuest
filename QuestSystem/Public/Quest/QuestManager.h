// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "EasyQuestAsset.h"
#include "EasyQuestRuntimeGraph.h"
#include "EasyQuestNodeInfo.h"

#include "Quest/QuestTrackerWidget.h"
#include "Quest/QuestTriggerActor.h"

#include "DlgSystem/DlgMemory.h"

#include "QuestManager.generated.h"

class UBaseQuest;

UCLASS(ClassGroup = (Custom), config = Game)
class WHISKEREDAWAY_API UQuestManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Subsystem init functions
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// Quest initialization functions
	UFUNCTION(BlueprintCallable, Category="Quest Functions")
	UBaseQuest* CreateQuest(const FString& Name, 
	 				 const FString& Dialogue,
					 const TArray<UBaseQuest*>& Dependencies);

	void ActivateQuest(UBaseQuest* Quest);

	// Getters
	UFUNCTION(BlueprintCallable, Category="Quest Manager Functions")
	UBaseQuest* GetActiveQuest();

	UFUNCTION(BlueprintCallable, Category="Quest Manager Functions")
	TArray<UBaseQuest*> GetCompletedQuests();

	UFUNCTION(BlueprintCallable, Category="Quest Manager Functions")
	TArray<UBaseQuest*> GetAvailableQuests();

	UFUNCTION(BlueprintCallable, Category="Quest Manager Functions")
	void CheckPendingQuests();

	UFUNCTION(BlueprintCallable, Category="Quest Manager Functions")
	TArray<UBaseQuest*> GetQuestPool() const;

	UFUNCTION(BlueprintCallable, Category = "Quest Manager")
	UQuestTrackerWidget* GetQuestTrackerWidget();

	UFUNCTION(BlueprintCallable, Category = "Quest Manager")
	AQuestTriggerActor* GetActiveTriggerActor() { return ActiveTriggerActor; }

	UFUNCTION(BlueprintCallable, Category = "Quest Manager")
	void SetActiveTriggerActor(AQuestTriggerActor* Trigger) { ActiveTriggerActor = Trigger; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest Manager Variables")
	TArray<UBaseQuest*> QuestPool;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest Manager Variables")
	TObjectPtr<UBaseQuest> CurrentQuest;

	UFUNCTION(BlueprintCallable, Category="Quest Manager Functions")
	void OnQuestCompleted(UBaseQuest* CompletedQuest);

public: //From EasyQuest helper funciton 

	// Load quests from graph
	void LoadQuestsFromAsset(UEasyQuestAsset* QuestAsset);

	void SetCurrentQuest(UBaseQuest* Quest) { CurrentQuest = Quest; }

public: // Saving and Loading functions

	UFUNCTION(BlueprintCallable, Category = "Quest Manager")
	void SaveProgress();

	UFUNCTION(BlueprintCallable, Category = "Quest Manager")
	void LoadProgress();

	UFUNCTION(BlueprintCallable, Category = "Quest Manager")
	void ResetProgress();
private:

	// Debug overlay — manager just holds the pointer and calls Start/Stop
	TSharedPtr<class SQuestDebugOverlay> QuestDebugOverlay;

	void OnLevelChanged(const FString& MapName);

	UPROPERTY()
	UQuestTrackerWidget* QuestTrackerWidget;

	UPROPERTY()
	AQuestTriggerActor* ActiveTriggerActor;

	void OnFirstTick(UWorld* World, ELevelTick TickType, float DeltaSeconds);
	bool bOverlayStarted = false;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
	bool HasSaveGame() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
	FDateTime GetSaveDateTime() const;
};
