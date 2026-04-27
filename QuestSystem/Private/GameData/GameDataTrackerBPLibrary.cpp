#include "GameData/GameDataTrackerBPLibrary.h"
#include "GameData/GameDataTrackerSubsystem.h"
#include "GameData/GameDataTrackerSettings.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

UGameDataTrackerSubsystem* UGameDataTrackerBPLibrary::GetGameDataTracker(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;
    UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);
    if (!GI) return nullptr;
    return GI->GetSubsystem<UGameDataTrackerSubsystem>();
}

TArray<FString> UGameDataTrackerBPLibrary::GetTrackerKeyOptions()
{
    TArray<FString> Result;
    const UGameDataTrackerSettings* Settings = GetDefault<UGameDataTrackerSettings>();

    if (!Settings || Settings->DefinitionTable.IsNull()) return Result;

    UDataTable* Table = Settings->DefinitionTable.LoadSynchronous();

    if (!Table) return Result;

    for (const FName& RowName : Table->GetRowNames())
    {
        Result.Add(RowName.ToString());
    }
    return Result;
}

bool UGameDataTrackerBPLibrary::QuickRecordInt(const UObject* WorldContextObject, FName Key, int32 Value)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    return Tracker ? Tracker->RecordInt(Key, Value) : false;
}

bool UGameDataTrackerBPLibrary::QuickRecordFloat(const UObject* WorldContextObject, FName Key, float Value)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    return Tracker ? Tracker->RecordFloat(Key, Value) : false;
}

bool UGameDataTrackerBPLibrary::QuickRecordBool(const UObject* WorldContextObject, FName Key, bool Value)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    return Tracker ? Tracker->RecordBool(Key, Value) : false;
}

bool UGameDataTrackerBPLibrary::QuickRecordString(const UObject* WorldContextObject, FName Key, const FString& Value)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    return Tracker ? Tracker->RecordString(Key, Value) : false;
}

bool UGameDataTrackerBPLibrary::QuickRecordVector(const UObject* WorldContextObject, FName Key, FVector Value)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    return Tracker ? Tracker->RecordVector(Key, Value) : false;
}

bool UGameDataTrackerBPLibrary::QuickIncrementInt(const UObject* WorldContextObject, FName Key, int32 Amount)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    return Tracker ? Tracker->IncrementInt(Key, Amount) : false;
}

bool UGameDataTrackerBPLibrary::QuickSetCatRef(const UObject* WorldContextObject, FName Key, ACat* Cat)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    return Tracker ? Tracker->SetCatRef(Key, Cat) : false;
}

int32 UGameDataTrackerBPLibrary::QuickGetInt(const UObject* WorldContextObject, FName Key)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    int32 Value = 0;
    if (Tracker) Tracker->GetInt(Key, Value);
    return Value;
}

float UGameDataTrackerBPLibrary::QuickGetFloat(const UObject* WorldContextObject, FName Key)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    float Value = 0.f;
    if (Tracker) Tracker->GetFloat(Key, Value);
    return Value;
}

bool UGameDataTrackerBPLibrary::QuickGetBool(const UObject* WorldContextObject, FName Key)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    bool Value = false;
    if (Tracker) Tracker->GetBool(Key, Value);
    return Value;
}

FString UGameDataTrackerBPLibrary::QuickGetString(const UObject* WorldContextObject, FName Key)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    FString Value;
    if (Tracker) Tracker->GetString(Key, Value);
    return Value;
}

ACat* UGameDataTrackerBPLibrary::QuickGetCatRef(const UObject* WorldContextObject, FName Key)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    ACat* Cat = nullptr;
    if (Tracker) Tracker->GetCatRef(Key, Cat);
    return Cat;
}

FVector UGameDataTrackerBPLibrary::QuickGetVector(const UObject* WorldContextObject, FName Key)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    FVector Value = FVector::ZeroVector;
    if (Tracker) Tracker->GetVector(Key, Value);
    return Value;
}

TArray<FName> UGameDataTrackerBPLibrary::GetAllDefinedKeys(const UObject* WorldContextObject)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    if (!Tracker || !Tracker->GetDefinitionTable()) return {};
    return Tracker->GetDefinitionTable()->GetRowNames();
}

FString UGameDataTrackerBPLibrary::GetDebugSummary(const UObject* WorldContextObject)
{
    UGameDataTrackerSubsystem* Tracker = GetGameDataTracker(WorldContextObject);
    if (!Tracker) return TEXT("GameDataTracker: No subsystem found.");
    TMap<FName, FGameDataFieldValue> AllData = Tracker->GetAllData();
    if (AllData.IsEmpty()) return TEXT("GameDataTracker: No data recorded yet.");
    FString Result = TEXT("=== GameDataTracker Session Data ===\n");
    for (const auto& Pair : AllData)
    {
        FString ValueStr;
        switch (Pair.Value.DataType)
        {
        case ETrackedDataType::Integer: ValueStr = FString::FromInt(Pair.Value.IntValue); break;
        case ETrackedDataType::Float:   ValueStr = FString::SanitizeFloat(Pair.Value.FloatValue); break;
        case ETrackedDataType::Bool:    ValueStr = Pair.Value.BoolValue ? TEXT("true") : TEXT("false"); break;
        case ETrackedDataType::String:  ValueStr = Pair.Value.StringValue; break;
        case ETrackedDataType::CatRef:
            ValueStr = Pair.Value.LastCaughtCatRef.IsValid()
                ? Pair.Value.LastCaughtCatRef->GetName()
                : TEXT("Nullptr");
            break;
        case ETrackedDataType::Vector:
            ValueStr = FString::Printf(TEXT("(X=%.1f, Y=%.1f, Z=%.1f)"),
                Pair.Value.VectorValue.X,
                Pair.Value.VectorValue.Y,
                Pair.Value.VectorValue.Z);
            break;
        }
        Result += FString::Printf(TEXT("  [%s] = %s\n"), *Pair.Key.ToString(), *ValueStr);
    }
    return Result;
}