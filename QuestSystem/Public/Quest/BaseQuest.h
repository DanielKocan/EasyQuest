// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Core/Public/Misc/Guid.h"
#include "EasyQuestNodeInfo.h"
#include "BaseQuest.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8 
{
	Blocked   UMETA(DisplayName = "Blocked"),      // Has incomplete dependencies
	Available UMETA(DisplayName = "Available"),    // Dependencies complete, can be started
	Active    UMETA(DisplayName = "Active"),       // Currently active quest
	Complete  UMETA(DisplayName = "Complete"),     // Finished
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnableSignature, UBaseQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDisableSignature, UBaseQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompleteSignature, UBaseQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompleteBlueprintSignature, UBaseQuest*, Quest);

UCLASS(ClassGroup=(Custom))
class WHISKEREDAWAY_API UBaseQuest : public UObject
{
	GENERATED_BODY()

public:
	UBaseQuest(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Quest State functions
	void Enable();
	
	UFUNCTION(BlueprintCallable, Category="Quest Functions")
	void Disable();
	
	UFUNCTION(BlueprintCallable, Category="Quest Functions")
	void Complete();

	// Condition interface that you can set in blueprint nodes
	UFUNCTION(BlueprintNativeEvent, Category="Quest Functions")
	bool CheckStartCondition();
	
	// Condition interface that you can set in blueprint nodes
	UFUNCTION(BlueprintNativeEvent, Category="Quest Functions")
	bool CheckEndCondition();
	
	EQuestState GetState() const;

	// Blueprint-assignable events called after quest state functions
	UPROPERTY(BlueprintAssignable, Category="Quest Events")
	FOnEnableSignature EnableEvent;

	UPROPERTY(BlueprintAssignable, Category="Quest Events")
	FOnDisableSignature DisableEvent;
	
	UPROPERTY(BlueprintAssignable, Category="Quest Events")
	FOnCompleteSignature CompleteEvent;
	
	UPROPERTY(BlueprintAssignable, Category="Quest Events")
	FOnCompleteBlueprintSignature CompleteEventBlueprint;
	
	// Quest initialization
	void Initialize(const FString& _Name, 
					const FString& _Dialogue,
					const TArray<UBaseQuest*>& _Dependencies);

	bool DependentQuestsFinished() const;

	// Get quest name
	UFUNCTION(BlueprintPure, Category = "Quest Functions")
	FString GetQuestName() const { return Name; }

	UFUNCTION(BlueprintPure, Category = "Quest Functions")
	FString GetQuestDescription() const { return Dialogue; }

	UFUNCTION(BlueprintPure, Category = "Quest Functions")
	FGuid GetQuestIdentifier() const { return Identifier; }

	UFUNCTION(BlueprintPure, Category = "Quest Functions")
	bool IsAvailable() const
	{
		return DependentQuestsFinished() && State != EQuestState::Active && State != EQuestState::Complete;
	}

	// Add a dependency after creation
	inline void AddDependency(UBaseQuest* DependencyQuest)
	{
		if (DependencyQuest && !Dependencies.Contains(DependencyQuest))
		{
			Dependencies.Add(DependencyQuest);
		}
	}


	const TArray<UBaseQuest*>& GetDependencies() const { return Dependencies; }

	void SetState(EQuestState NewState) { State = NewState; }

public:

	UPROPERTY(BlueprintReadOnly, Category = "Quest Variables")
	bool bAllCriteriaMet = false;

	void SetAllCriteriaMet(bool bMet) { bAllCriteriaMet = bMet; }

	UFUNCTION(BlueprintPure, Category = "Quest Functions")
	bool IsAllCriteriaMet() const { return bAllCriteriaMet; }

	void SetVisible(bool bVisible) { bIsVisible = bVisible; }

	UFUNCTION(BlueprintPure, Category = "Quest Functions")
	bool IsVisible() const { return bIsVisible; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest Variables")
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest Variables")
	FString Dialogue;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest Variables")
	FGuid Identifier;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest Variables")
	EQuestState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest Variables")
	TArray<UBaseQuest*> Dependencies;

private:
	bool IsQuestValid() const;

	UPROPERTY()
	bool bIsVisible = false;  // hidden by default
};
