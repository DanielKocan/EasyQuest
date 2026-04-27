#include "../../Public/Quest/QuestBoardActor.h"
#include "../../Public/Quest/QuestBoardWidget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AQuestBoardActor::AQuestBoardActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create mesh for the physical board
    BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
    RootComponent = BoardMesh;

    // Create trigger box
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetBoxExtent(FVector(200.0f, 200.0f, 100.0f)); // Adjust size as needed
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AQuestBoardActor::BeginPlay()
{
    Super::BeginPlay();

    // Bind overlap events
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AQuestBoardActor::OnPlayerEnter);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AQuestBoardActor::OnPlayerLeave);

    // Get player controller
    PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void AQuestBoardActor::OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if it's the player character
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (OtherActor == PlayerCharacter)
    {
        ShowQuestBoard();
    }
}

void AQuestBoardActor::OnPlayerLeave(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // Check if it's the player character
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (OtherActor == PlayerCharacter)
    {
        HideQuestBoard();
    }
}

void AQuestBoardActor::ShowQuestBoard()
{
    if (!PlayerController || !QuestBoardWidgetClass) return;

    // Create widget if it doesn't exist
    if (!QuestBoardWidget)
    {
        QuestBoardWidget = CreateWidget<UQuestBoardWidget>(PlayerController, QuestBoardWidgetClass);
    }

    // Show widget
    if (QuestBoardWidget && !QuestBoardWidget->IsInViewport())
    {
        // REFRESH before showing
        QuestBoardWidget->RefreshQuestList();

        QuestBoardWidget->AddToViewport();

        // Show mouse cursor
        PlayerController->bShowMouseCursor = true;
        PlayerController->SetInputMode(FInputModeGameAndUI());

        UE_LOG(LogTemp, Log, TEXT("Quest Board opened"));
    }
}

void AQuestBoardActor::HideQuestBoard()
{
    if (QuestBoardWidget && QuestBoardWidget->IsInViewport())
    {
        QuestBoardWidget->RemoveFromParent();

        // Hide mouse cursor
        if (PlayerController)
        {
            PlayerController->bShowMouseCursor = false;
            PlayerController->SetInputMode(FInputModeGameOnly());
        }

        UE_LOG(LogTemp, Log, TEXT("Quest Board closed"));
    }
}