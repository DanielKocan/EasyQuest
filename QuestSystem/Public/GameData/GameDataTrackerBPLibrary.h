#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameDataTrackerTypes.h"
#include "GameDataTrackerBPLibrary.generated.h"


/**
 * UGameDataTrackerBPLibrary
 *
 * Blueprint-friendly static functions for the GameDataTracker system.
 * These wrap the subsystem calls so Blueprint users don't have to
 * manually get the subsystem each time.
 *
 * The Key parameter appears as a dropdown in Blueprint because it reads
 * from the DataTable via GetOptions meta — see note below on setup.
 *
 * NOTE ON DROPDOWN:
 * Unreal's native Blueprint dropdown for FName from a DataTable requires
 * a custom Detail Customization or using a FDataTableRowHandle.
 * The cleanest practical approach for Blueprint users is:
 *   - Use "Get All Data Table Row Names" node to populate a variable
 *   - Or use the helper GetAllDefinedKeys() to fill a dropdown in a widget
 *
 * For a true compile-time dropdown, a small Editor module with
 * IPropertyTypeCustomization is the proper approach (can be added later).
 */
UCLASS()
class WHISKEREDAWAY_API UGameDataTrackerBPLibrary : public UBlueprintFunctionLibrary
{
public:
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameDataTracker",
        meta = (DisplayName = "Get Game Data Tracker", WorldContext = "WorldContextObject"))
    static class UGameDataTrackerSubsystem* GetGameDataTracker(const UObject* WorldContextObject);

    // Editor calls this to populate the Key dropdown
    UFUNCTION()
    static TArray<FString> GetTrackerKeyOptions();

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Record Int", WorldContext = "WorldContextObject"))
    static bool QuickRecordInt(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key,
        int32 Value);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Record Float", WorldContext = "WorldContextObject"))
    static bool QuickRecordFloat(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key,
        float Value);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Record Bool", WorldContext = "WorldContextObject"))
    static bool QuickRecordBool(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key,
        bool Value);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Record String", WorldContext = "WorldContextObject"))
    static bool QuickRecordString(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key,
        const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Increment Int", WorldContext = "WorldContextObject"))
    static bool QuickIncrementInt(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key,
        int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Set Cat Ref", WorldContext = "WorldContextObject"))
    static bool QuickSetCatRef(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key,
        ACat* Cat);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Record Vector", WorldContext = "WorldContextObject"))
    static bool QuickRecordVector(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions"))FName Key, 
        FVector Value);

public: //Getters

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameDataTracker",
        meta = (DisplayName = "Get Int", WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    static int32 QuickGetInt(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameDataTracker",
        meta = (DisplayName = "Get Float", WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    static float QuickGetFloat(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameDataTracker",
        meta = (DisplayName = "Get Bool", WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    static bool QuickGetBool(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameDataTracker",
        meta = (DisplayName = "Get String", WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    static FString QuickGetString(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (DisplayName = "Get Cat Ref", WorldContext = "WorldContextObject"))
    static ACat* QuickGetCatRef(
        const UObject* WorldContextObject,
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameDataTracker",
        meta = (DisplayName = "Get Vector", WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    static FVector QuickGetVector(const UObject* WorldContextObject, 
        UPARAM(meta = (GetOptions = "WhiskeredAway.GameDataTrackerBPLibrary.GetTrackerKeyOptions")) FName Key);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (WorldContext = "WorldContextObject"))
    static TArray<FName> GetAllDefinedKeys(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category = "GameDataTracker",
        meta = (WorldContext = "WorldContextObject"))
    static FString GetDebugSummary(const UObject* WorldContextObject);
};
