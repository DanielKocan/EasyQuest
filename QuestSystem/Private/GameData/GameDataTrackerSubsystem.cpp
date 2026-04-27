#include "GameData/GameDataTrackerSubsystem.h"
#include "GameData/GameDataTrackerSettings.h"
#include "GameData/GameDataTrackerDebugOverlay.h"
#include "GameData/GameDataTrackerKeys.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"

// -------------------------------------------------------------------------
// USubsystem interface
// -------------------------------------------------------------------------

void UGameDataTrackerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    const UGameDataTrackerSettings* Settings = GetDefault<UGameDataTrackerSettings>();
    if (Settings && !Settings->DefinitionTable.IsNull())
    {
        UDataTable* Table = Settings->DefinitionTable.LoadSynchronous();
        InitializeWithTable(Table);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameDataTracker: No DataTable assigned in Project Settings."));
    }

    // Start debug overlay if enabled in Project Settings
#if !UE_BUILD_SHIPPING
    if (Settings && Settings->bShowDebugOverlay)
    {
        // Viewport doesn't exist yet at subsystem init time.
        // Delay until the first world PostBeginPlay when viewport is guaranteed ready.
        FWorldDelegates::OnWorldPostActorTick.AddUObject(
            this, &UGameDataTrackerSubsystem::OnFirstTick);
    }
#endif
}

void UGameDataTrackerSubsystem::OnFirstTick(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
    if (bOverlayStarted) return;

    UGameInstance* GI = GetGameInstance();
    if (!GI) return;

    UGameViewportClient* Viewport = GI->GetGameViewportClient();
    if (!Viewport) return; // not ready yet, will retry next tick

    // Viewport is ready — start the overlay and unsubscribe
    bOverlayStarted = true;
    FWorldDelegates::OnWorldPostActorTick.RemoveAll(this);

    DebugOverlay = SNew(SGameDataTrackerDebugOverlay);
    DebugOverlay->Start(GI);

    UE_LOG(LogTemp, Warning, TEXT("GameDataTracker: Debug overlay started."));
}

void UGameDataTrackerSubsystem::Deinitialize()
{
    FWorldDelegates::OnWorldPostActorTick.RemoveAll(this);
    // Stop and clean up debug overlay
    if (DebugOverlay.IsValid())
    {
        DebugOverlay->Stop();
        DebugOverlay.Reset();
    }

    Super::Deinitialize();
}

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------

void UGameDataTrackerSubsystem::InitializeWithTable(UDataTable* InDefinitionTable)
{
    if (!InDefinitionTable)
    {
        UE_LOG(LogTemp, Error, TEXT("GameDataTracker: InitializeWithTable called with null DataTable."));
        return;
    }

    // Validate the row struct is correct
    if (InDefinitionTable->GetRowStruct() != FGameDataFieldDefinition::StaticStruct())
    {
        UE_LOG(LogTemp, Error,
            TEXT("GameDataTracker: DataTable '%s' does not use FGameDataFieldDefinition as its row struct."),
            *InDefinitionTable->GetName());
        return;
    }

    DefinitionTable = InDefinitionTable;
    RecordedData.Empty();

    // Pre-populate all fields with default values so they show in the overlay immediately
    for (const FName& RowName : InDefinitionTable->GetRowNames())
    {
        const FGameDataFieldDefinition* Row = InDefinitionTable->FindRow<FGameDataFieldDefinition>(RowName, TEXT(""));
        if (!Row) continue;

        FGameDataFieldValue DefaultValue;
        DefaultValue.DataType = Row->DataType;
        // int = 0, float = 0.f, bool = false, string = "" — already set by struct defaults

        RecordedData.Add(RowName, DefaultValue);
    }


    UE_LOG(LogTemp, Log,
        TEXT("GameDataTracker: Initialized with table '%s' — %d fields defined."),
        *InDefinitionTable->GetName(),
        InDefinitionTable->GetRowNames().Num());

#if !UE_BUILD_SHIPPING 
    // --- Log which keys from the DataTable have C++ constants and which don't --- 
    const TArray<FName> RowNames = InDefinitionTable->GetRowNames();

    UE_LOG(LogTemp, Log, TEXT("GameDataTracker: ===== Key Binding Report ====="));

    for (const FName& RowName : RowNames)
    {
        const bool bHasCppConstant = GameDataTrackerKeys::AllKeys.Contains(RowName);
        if (bHasCppConstant)
        {
            UE_LOG(LogTemp, Log, TEXT("GameDataTracker:  [BOUND]   '%s' - has C++ constant"), *RowName.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("GameDataTracker:  [UNBOUND] '%s' - Blueprint only, no C++ constant"), *RowName.ToString());
        }
    }

    // Also check the reverse — C++ constants that are missing from the DataTable
    for (const FName& CppKey : GameDataTrackerKeys::AllKeys)
    {
        if (!RowNames.Contains(CppKey))
        {
            UE_LOG(LogTemp, Error, TEXT("GameDataTracker:  [MISSING] '%s' - has C++ constant but NOT in DataTable!"), *CppKey.ToString());
        }
    }

    UE_LOG(LogTemp, Log, TEXT("GameDataTracker: =============================="));
#endif
}

// -------------------------------------------------------------------------
// Record functions
// -------------------------------------------------------------------------

bool UGameDataTrackerSubsystem::RecordInt(FName Key, int32 Value)
{
    FGameDataFieldValue FieldValue;
    FieldValue.DataType = ETrackedDataType::Integer;
    FieldValue.IntValue = Value;
    return RecordValue(Key, ETrackedDataType::Integer, FieldValue);
}

bool UGameDataTrackerSubsystem::RecordFloat(FName Key, float Value)
{
    FGameDataFieldValue FieldValue;
    FieldValue.DataType = ETrackedDataType::Float;
    FieldValue.FloatValue = Value;
    return RecordValue(Key, ETrackedDataType::Float, FieldValue);
}

bool UGameDataTrackerSubsystem::RecordBool(FName Key, bool Value)
{
    FGameDataFieldValue FieldValue;
    FieldValue.DataType = ETrackedDataType::Bool;
    FieldValue.BoolValue = Value;
    return RecordValue(Key, ETrackedDataType::Bool, FieldValue);
}

bool UGameDataTrackerSubsystem::RecordString(FName Key, const FString& Value)
{
    FGameDataFieldValue FieldValue;
    FieldValue.DataType = ETrackedDataType::String;
    FieldValue.StringValue = Value;
    return RecordValue(Key, ETrackedDataType::String, FieldValue);
}

bool UGameDataTrackerSubsystem::SetCatRef(FName Key, ACat* Cat)
{
    FGameDataFieldValue FieldValue;
    FieldValue.DataType = ETrackedDataType::CatRef;
    FieldValue.LastCaughtCatRef = Cat;
    return RecordValue(Key, ETrackedDataType::CatRef, FieldValue);
}

bool UGameDataTrackerSubsystem::IncrementInt(FName Key, int32 Amount)
{
    if (!IsKeyValid(Key))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("unknown key"));
        return false;
    }

    // If not recorded yet, starts from 0
    int32 Current = 0;
    if (FGameDataFieldValue* Existing = RecordedData.Find(Key))
    {
        if (Existing->DataType != ETrackedDataType::Integer)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("GameDataTracker: IncrementInt — key '%s' is not an Integer field."),
                *Key.ToString());
            return false;
        }
        Current = Existing->IntValue;
    }

    return RecordInt(Key, Current + Amount);
}

// -------------------------------------------------------------------------
// Read functions
// -------------------------------------------------------------------------

bool UGameDataTrackerSubsystem::GetInt(FName Key, int32& OutValue) const
{
    const FGameDataFieldValue* Value = RecordedData.Find(Key);
    if (Value && Value->DataType == ETrackedDataType::Integer)
    {
        OutValue = Value->IntValue;
        return true;
    }
    return false;
}

bool UGameDataTrackerSubsystem::GetFloat(FName Key, float& OutValue) const
{
    const FGameDataFieldValue* Value = RecordedData.Find(Key);
    if (Value && Value->DataType == ETrackedDataType::Float)
    {
        OutValue = Value->FloatValue;
        return true;
    }
    return false;
}

bool UGameDataTrackerSubsystem::GetBool(FName Key, bool& OutValue) const
{
    const FGameDataFieldValue* Value = RecordedData.Find(Key);
    if (Value && Value->DataType == ETrackedDataType::Bool)
    {
        OutValue = Value->BoolValue;
        return true;
    }
    return false;
}

bool UGameDataTrackerSubsystem::GetString(FName Key, FString& OutValue) const
{
    const FGameDataFieldValue* Value = RecordedData.Find(Key);
    if (Value && Value->DataType == ETrackedDataType::String)
    {
        OutValue = Value->StringValue;
        return true;
    }
    return false;
}

bool UGameDataTrackerSubsystem::GetCatRef(FName Key, ACat*& OutCat)
{
    const FGameDataFieldValue* Value = RecordedData.Find(Key);
    if (Value && Value->DataType == ETrackedDataType::CatRef)
    {
        OutCat = Value->LastCaughtCatRef.Get();  // nullptr if destroyed
        return OutCat != nullptr;
    }
    OutCat = nullptr;
    return false;
}

TMap<FName, FGameDataFieldValue> UGameDataTrackerSubsystem::GetAllData() const
{
    return RecordedData;
}

bool UGameDataTrackerSubsystem::IsKeyValid(FName Key) const
{
    if (!DefinitionTable) return false;
    return DefinitionTable->FindRow<FGameDataFieldDefinition>(Key, TEXT("")) != nullptr;
}

// -------------------------------------------------------------------------
// Utility
// -------------------------------------------------------------------------

void UGameDataTrackerSubsystem::ResetAllData()
{
    RecordedData.Empty();
    UE_LOG(LogTemp, Log, TEXT("GameDataTracker: All recorded data cleared."));
}

bool UGameDataTrackerSubsystem::GetFieldDefinition(FName Key, FGameDataFieldDefinition& OutDefinition) const
{
    if (!DefinitionTable) return false;

    const FGameDataFieldDefinition* Row = DefinitionTable->FindRow<FGameDataFieldDefinition>(Key, TEXT("GetFieldDefinition"));
    if (Row)
    {
        OutDefinition = *Row;
        return true;
    }
    return false;
}

void UGameDataTrackerSubsystem::ValidateKeys(const TArray<FName>& KeysToValidate) const
{
    for (const FName& Key : KeysToValidate)
    {
        if (!IsKeyValid(Key))
        {
            UE_LOG(LogTemp, Error,
                TEXT("GameDataTracker: Key '%s' defined in GameDataTrackerKeys.h is MISSING from the DataTable. Please add it."),
                *Key.ToString());
        }
    }
}

bool UGameDataTrackerSubsystem::RecordValue(FName Key, ETrackedDataType ExpectedType, const FGameDataFieldValue& Value)
{
    if (!DefinitionTable)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("GameDataTracker: Tried to record key '%s' but no DataTable has been set. Call InitializeWithTable first."),
            *Key.ToString());
        return false;
    }

    const FGameDataFieldDefinition* Definition = DefinitionTable->FindRow<FGameDataFieldDefinition>(Key, TEXT("RecordValue"));
    if (!Definition)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("GameDataTracker: Key '%s' not found in DataTable '%s'. Add a row with this name."),
            *Key.ToString(),
            *DefinitionTable->GetName());
        return false;
    }

    if (Definition->DataType != ExpectedType)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("GameDataTracker: Type mismatch for key '%s'. DataTable says type %d, but you are recording type %d."),
            *Key.ToString(),
            (int32)Definition->DataType,
            (int32)ExpectedType);
        return false;
    }

    RecordedData.Add(Key, Value);
    return true;
}

bool UGameDataTrackerSubsystem::RecordVector(FName Key, FVector Value)
{
    FGameDataFieldValue FieldValue;
    FieldValue.DataType = ETrackedDataType::Vector;
    FieldValue.VectorValue = Value;
    return RecordValue(Key, ETrackedDataType::Vector, FieldValue);
}


bool UGameDataTrackerSubsystem::GetVector(FName Key, FVector& OutValue) const
{
    const FGameDataFieldValue* Value = RecordedData.Find(Key);
    if (Value && Value->DataType == ETrackedDataType::Vector)
    {
        OutValue = Value->VectorValue;
        return true;
    }
    return false;
}