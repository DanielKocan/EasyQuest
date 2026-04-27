#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameData/GameDataTrackerKeys.h"
#include "GameDataTrackerTypes.h"
#include "GameDataTrackerSubsystem.generated.h"

/**
 * UGameDataTrackerSubsystem (usage example in GameDataTrackerKeys.h)
 *
 * HOW TO USE IN C++:
 *   UGameDataTrackerSubsystem* Tracker = GetGameInstance()->GetSubsystem<UGameDataTrackerSubsystem>();
 *   Tracker->RecordInt("CatCount", 5);
 *   Tracker->RecordFloat("LevelDifficulty", 3.5f);
 *
 * HOW TO USE IN BLUEPRINT:
 *   Use the "Get Game Data Tracker" node, then call Record Int / Record Float etc.
 *   The Key dropdown is populated from your GameDataTrackerTable DataTable.
 *
 * SETUP:.
 *   1. Create a DataTable asset using FGameDataFieldDefinition as the row struct. (ONLY ONE SHOULD BE! CHECK BEFORE CREATING, POSSIBILY IT IS ALREADY THERE, so you can skipp then all these steps and use already existing DataTable)
 *   2. Set the DataTable reference in Project Settings or pass it to InitializeWithTable().
 *   3. Add rows — each row name becomes a valid key (e.g. "CatCount", "LevelDifficulty").
 *   4. Check "GameDataTrackerKeys.h" if you want to use it in C++ also.
 */
UCLASS()
class WHISKEREDAWAY_API  UGameDataTrackerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public: //USubsystem interface

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private: // function and variables now only for debug overlay

    void OnFirstTick(UWorld* World, ELevelTick TickType, float DeltaSeconds);
    bool bOverlayStarted = false;

public: // Record functions — called from C++ or Blueprint to store a value

	UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
	void InitializeWithTable(UDataTable* InDefinitionTable);

    /** Record an integer value. Key must exist in the DataTable as Integer type. */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker", meta = (DisplayName = "Record Int"))
    bool RecordInt(FName Key, int32 Value);

    /** Record a float value. Key must exist in the DataTable as Float type. */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker", meta = (DisplayName = "Record Float"))
    bool RecordFloat(FName Key, float Value);

    /** Record a bool value. Key must exist in the DataTable as Bool type. */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker", meta = (DisplayName = "Record Bool"))
    bool RecordBool(FName Key, bool Value);

    /** Record a string value. Key must exist in the DataTable as String type. */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker", meta = (DisplayName = "Record String"))
    bool RecordString(FName Key, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker", meta = (DisplayName = "Record Vector"))
    bool RecordVector(FName Key, FVector Value);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool SetCatRef(FName Key, ACat* Cat);

    // Increment an integer field by Amount (default 1).
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker", meta = (DisplayName = "Increment Int"))
    bool IncrementInt(FName Key, int32 Amount = 1);

public:  // Read functions

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool GetInt(FName Key, int32& OutValue) const;

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool GetFloat(FName Key, float& OutValue) const;

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool GetBool(FName Key, bool& OutValue) const;

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool GetString(FName Key, FString& OutValue) const;

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool GetCatRef(FName Key, ACat*& OutCat);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool GetVector(FName Key, FVector& OutValue) const;

    /** Returns all currently recorded data as a snapshot map */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    TMap<FName, FGameDataFieldValue> GetAllData() const;

    /** Returns true if a key is registered in the DataTable */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool IsKeyValid(FName Key) const;

public:  // Utility

    /** Clears all recorded values (does NOT clear the field definitions) */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    void ResetAllData();

    /** Returns the definition for a field (type, category, description) */
    UFUNCTION(BlueprintCallable, Category = "GameDataTracker")
    bool GetFieldDefinition(FName Key, FGameDataFieldDefinition& OutDefinition) const;

    /** Validates that all provided keys exist in the DataTable. Logs errors for missing ones. */
    void ValidateKeys(const TArray<FName>& KeysToValidate) const;

    /** Access the raw DataTable if you need to iterate rows directly */
    UDataTable* GetDefinitionTable() const { return DefinitionTable; }

private:

    /** The DataTable defining all trackable fields */
    UPROPERTY()
    UDataTable* DefinitionTable;

    /** Runtime storage: key -> current value */
    TMap<FName, FGameDataFieldValue> RecordedData;

    // Separate storage for cat refs - bypasses DataTable type validation we dont need it 
    // This is intentionally separate from RecordedData because cat refs are runtime-only, non-serializable, and don't belong in the DataTable schema — the DataTable is for designer-defined value types. The two maps stay cleanly separated by purpose.
    TMap<FName, TWeakObjectPtr<ACat>> RecordedCatRefs;

    /**
     * Internal helper: tries to record a value after validating key + type match.
     * Returns false and logs a warning if the key is unknown or the type is wrong.
     */
    bool RecordValue(FName Key, ETrackedDataType ExpectedType, const FGameDataFieldValue& Value);

    // Debug overlay — subsystem just holds the pointer and calls Start/Stop
    TSharedPtr<class SGameDataTrackerDebugOverlay> DebugOverlay;
};
