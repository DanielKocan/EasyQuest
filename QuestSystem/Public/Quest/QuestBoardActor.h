#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestBoardActor.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UQuestBoardWidget;

UCLASS()
class WHISKEREDAWAY_API AQuestBoardActor : public AActor
{
    GENERATED_BODY()

public:
    AQuestBoardActor();

protected:
    virtual void BeginPlay() override;

    // Visual mesh (optional - for the physical board)
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* BoardMesh;

    // Trigger box for player proximity
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerBox;

    // The quest board widget class
    UPROPERTY(EditAnywhere, Category = "Quest Board")
    TSubclassOf<UQuestBoardWidget> QuestBoardWidgetClass;

private:
    UFUNCTION()
    void OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerLeave(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void ShowQuestBoard();
    void HideQuestBoard();

    UPROPERTY()
    UQuestBoardWidget* QuestBoardWidget;

    UPROPERTY()
    APlayerController* PlayerController;
};