// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/BaseQuest.h"

UBaseQuest::UBaseQuest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Name = "";
	Dialogue = "";
	State = EQuestState::Blocked; //by default quest is blocked
}

void UBaseQuest::Initialize(const FString& _Name, const FString& _Dialogue, const TArray<UBaseQuest*>& _Dependencies)
{
	Name = _Name;
	Dialogue = _Dialogue;
	Dependencies = _Dependencies;
	Identifier = FGuid::NewGuid();

	// Set initial state based on dependencies
	if (Dependencies.Num() == 0)
	{
		State = EQuestState::Available;  // No dependencies = Available immediately
	}
	else
	{
		State = EQuestState::Blocked;    // Has dependencies = Blocked until they're done
	}
}

void UBaseQuest::Enable()
{
	if(!IsQuestValid()) return;
	if(!CheckStartCondition()) return;
	// Check dependencies
	if (!DependentQuestsFinished())
	{
		// If dependencies not finished, mark as Blocked
		if (State != EQuestState::Blocked)
		{
			State = EQuestState::Blocked;
			UE_LOG(LogTemp, Log, TEXT("Quest '%s' is blocked by dependencies"), *Name);
		}
		return;
	}

	// Dependencies finished - can be started
	if (State == EQuestState::Blocked || State == EQuestState::Available)
	{
		State = EQuestState::Active;
		EnableEvent.Broadcast(this);
		UE_LOG(LogTemp, Log, TEXT("Quest '%s' activated"), *Name);
	}
}

void UBaseQuest::Disable() // you cant sidable blocked quest
{
	if(!IsQuestValid()) return;

	if(State == EQuestState::Active)
	{
		State = EQuestState::Available;

		DisableEvent.Broadcast(this);
	}
}

void UBaseQuest::Complete()
{
	if(!IsQuestValid()) return;
	if(!CheckEndCondition()) return;

	if(State == EQuestState::Active)
	{
		State = EQuestState::Complete;
		bAllCriteriaMet = false;
		CompleteEvent.Broadcast(this);
		CompleteEventBlueprint.Broadcast(this);
	}
}

bool UBaseQuest::CheckStartCondition_Implementation()
{
	return true;
}

bool UBaseQuest::CheckEndCondition_Implementation()
{
	return true;
}

EQuestState UBaseQuest::GetState() const
{
	return State;
}

bool UBaseQuest::IsQuestValid() const
{
	return !Name.IsEmpty() && Identifier.IsValid();	
}

bool UBaseQuest::DependentQuestsFinished() const
{
	for(auto& Quest : Dependencies)
	{
		if(!IsValid(Quest))
		{
			UE_LOG(LogTemp, Error, TEXT("Dependent quest is not valid"));
			return false;
		}

		if(Quest->State != EQuestState::Complete) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Dependent quest is incomplete."));
			return false;
		}
	}

	return true;
}
