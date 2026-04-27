#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Developer/ChadCatJoey/Cat.h"
#include "GameDataTrackerTypes.generated.h"

/**
 * The supported data types for a tracked field.
 * Add more here if needed (e.g. FString, bool).
 */
UENUM(BlueprintType)
enum class ETrackedDataType : uint8
{
    Integer        UMETA(DisplayName = "Integer (int32)"),
    Float          UMETA(DisplayName = "Float"),
    Bool           UMETA(DisplayName = "Bool"),
    String         UMETA(DisplayName = "String"),
    Vector         UMETA(DisplayName = "Vector"),
    CatRef         UMETA(DisplayName = "CatRef")
};

/**
 * One row in the GameDataTrackerTable DataTable.
 * Designers add a row per field they want to track.
 *
 * Row Name  = the FName key used in code (e.g. "CatCount")
 * DataType  = what kind of value this field holds
 */
USTRUCT(BlueprintType)
struct FGameDataFieldDefinition : public FTableRowBase
{
    GENERATED_BODY()

    /** The type of data stored for this field */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameDataTracker")
    ETrackedDataType DataType = ETrackedDataType::Integer;

    /** Optional description so designers know what this field is for */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameDataTracker", meta = (MultiLine = true))
    FText Description;
};

/**
 * Holds the actual runtime value for a tracked field.
 * All types are stored here; only the one matching DataType is valid.
 */
USTRUCT(BlueprintType)
struct FGameDataFieldValue
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "GameDataTracker")
    ETrackedDataType DataType = ETrackedDataType::Integer;

    UPROPERTY(BlueprintReadOnly, Category = "GameDataTracker")
    int32 IntValue = 0;

    UPROPERTY(BlueprintReadOnly, Category = "GameDataTracker")
    float FloatValue = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "GameDataTracker")
    bool BoolValue = false;

    UPROPERTY(BlueprintReadOnly, Category = "GameDataTracker")
    FString StringValue;

    UPROPERTY(BlueprintReadOnly, Category = "GameDataTracker")
    TWeakObjectPtr<ACat> LastCaughtCatRef;

    UPROPERTY(BlueprintReadOnly, Category = "GameDataTracker")
    FVector VectorValue = FVector::ZeroVector;
};
