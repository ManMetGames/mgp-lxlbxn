#include "MyPlayerController.h"
#include "Engine/Engine.h" //for debugs
#include "Engine/World.h" //spawning actors/timers
#include "Components/DecalComponent.h"
#include "Materials/MaterialInterface.h"
#include "WProjectile.h"
#include "GameFramework/Pawn.h" 
#include "AIController.h" 
#include "NavigationSystem.h" 
#include "MyUserWidget.h"
#include "Kismet/GameplayStatics.h" 
#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
/**
*  Design goal - when W and Q are cast, aim projectile at cursor
*  itll fire a projectile a short distance
*  destroy itself at a certain range, stop player movement on its cast, 
*  re allow movement off cast, and be placed on cooldown
*  put a cooldown on UI
*/

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    // Keybind
    InputComponent->BindAction(
        "Q",
        IE_Pressed,
        this,
        &AMyPlayerController::OnQPressed
    );

    InputComponent->BindAction(
        "MoveCommand",
        IE_Pressed,
        this,
        &AMyPlayerController::OnMoveCommand
    );

    InputComponent->BindAction(
        "W",
        IE_Pressed,
        this,
        &AMyPlayerController::OnWPressed
    );

}

// Goal - when the game starts the UI should be displayed , mouse enabled
void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;

    MyUserWidget = CreateWidget<UMyUserWidget>(this, MyUserWidgetClass);

    MyUserWidget->AddToViewport(10);
    
}

// Goal - start timers and prep for ability to cast (deny if casting or on cd and stops movement) 
void AMyPlayerController::OnQPressed()
{
    // Block if ability is on cooldown or already casting
    if (bQOnCooldown || IsCasting())
    {
        return;
    }
    // Stop while castsing
    StopCurrentMovement();

    // Ensure no duplicate timers
    if (GetWorldTimerManager().IsTimerActive(QCooldownTickTimer))
    {
        return;
    }   

    // Start smooth cooldown ticking
    GetWorldTimerManager().SetTimer(
        QCooldownTickTimer,
        this,
        &AMyPlayerController::TickQCooldown,
        0.05f,
        true
    );
    
    // Start casting (movement lock)
    bQIsCasting = true;

    // End cast after short delay
    GetWorldTimerManager().SetTimer(
        QCastTimer,
        this,
        &AMyPlayerController::EndQCast,
        QCastDuration,
        false
    );

    // Start cooldown timer (longer)
    GetWorldTimerManager().SetTimer(
        QCooldownTimer,
        this,
        &AMyPlayerController::ResetQCooldown,
        QCooldownDuration,
        false
    );
}

void AMyPlayerController::ResetQCooldown()
{
    bQOnCooldown = false;

    // Stop tick for UI
    GetWorldTimerManager().ClearTimer(QCooldownTickTimer);

    if (MyUserWidget)
    {
        // So UI ends at 0s
        MyUserWidget->SetQCooldownPercent(0.0f);
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Cyan,
            TEXT("Q cooldown finished!")
        );
    }
}

// Goal - casts ability and start cooldowns and let move again
void AMyPlayerController::EndQCast()
{
    bQIsCasting = false;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn || !QProjectileClass)
    {
        return;
    }
    //Debug and Use Ccurrent pawn location
    FVector PawnLocation = ControlledPawn->GetActorLocation();

    // Get mouse hit
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        return;
    }

    // Lock Z to player's Z so that it doesnt go up or down
    FVector TargetLocation = Hit.ImpactPoint;
    TargetLocation.Z = PawnLocation.Z;

    FVector Direction = (TargetLocation - PawnLocation).GetSafeNormal();

    // Spawn slightly in front of pawn
    FVector ProjectileSpawnLocation = PawnLocation + Direction * 100.0f;
    FRotator SpawnRotation = Direction.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = ControlledPawn;

    GetWorld()->SpawnActor<AQProjectile>(
        QProjectileClass,
        ProjectileSpawnLocation,
        SpawnRotation,
        Params
    );
    // Start cooldown and track
    bQOnCooldown = true;
    QCooldownRemaining = QCooldownDuration;

    // Update UI when Q cast
    if (MyUserWidget)
    {
        MyUserWidget->SetQCooldownPercent(1.0f);
    }

    // Start cooldown timer (longer)
    GetWorldTimerManager().SetTimer(
        QCooldownTimer,
        this,
        &AMyPlayerController::ResetQCooldown,
        QCooldownDuration,
        false
    );
}

void AMyPlayerController::TickQCooldown()   
{
    // Decrease remaining time
    QCooldownRemaining -= 0.05f;
    UE_LOG(LogTemp, Warning, TEXT("Tick Q UI"));

    // Clamp to zero
    QCooldownRemaining = FMath::Max(QCooldownRemaining, 0.0f);

    float Percent = QCooldownRemaining / QCooldownDuration;

    // DEBUG: prove ticking
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            0.1f,
            FColor::Green,
            FString::Printf(TEXT("Tick Percent: %.2f"), Percent)
        );
    }

    // Update UI
    if (MyUserWidget)
    {
        MyUserWidget->SetQCooldownPercent(Percent);
    }
}

// So we can aim movement and abilities
bool AMyPlayerController::GetMouseWorldLocation(FVector& OutLocation) const
{
    FHitResult Hit;

    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        OutLocation = Hit.ImpactPoint;
        return true;
    }

    return false;
}

void AMyPlayerController::OnMoveCommand()
{
    // No movement if mid cast
    if (IsCasting())
    {
        return;
    }

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        APawn* ControlledPawn = GetPawn();
        if (!ControlledPawn)
        {
            return;
        }
        
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(
            this,
            Hit.ImpactPoint
        );
    }
}

void AMyPlayerController::StopCurrentMovement()
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    // Interupts click to move
    if (AAIController* AICon = Cast<AAIController>(ControlledPawn->GetController()))
    {
        AICon->StopMovement();
    }
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // While casting, continuously cancel movement
    if (bQIsCasting || bWIsCasting)
    {
        StopCurrentMovement();
    }
}

void AMyPlayerController::OnWPressed()
{   

    // Block if W is on cooldown
    if (bWOnCooldown || IsCasting())
    {
        return;
    }
    // Stop while casting
    StopCurrentMovement();

    bWIsCasting = true;

    GetWorldTimerManager().SetTimer(
        WCastTimer,
        this,
        &AMyPlayerController::EndWCast,
        WCastDuration,
        false
    );

    // Start ticking W cooldown UI
    GetWorldTimerManager().SetTimer(
        WCooldownTickTimer,
        this,
        &AMyPlayerController::TickWCooldown,
        0.05f,
        true
    );
}

void AMyPlayerController::ResetWCooldown()
{
    bWOnCooldown = false;

    // Safety: ensure UI resets
    if (MyUserWidget)
    {
        MyUserWidget->SetWCooldownPercent(0.0f);
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 1.5f, FColor::Green,
            TEXT("W cooldown finished")
        );
    }
}

void AMyPlayerController::TickWCooldown()
{
    WCooldownRemaining -= 0.05f;
    WCooldownRemaining = FMath::Max(WCooldownRemaining, 0.0f);
    UE_LOG(LogTemp, Warning, TEXT("Tick W UI"));

    float Percent = WCooldownRemaining / WCooldownDuration;

    if (MyUserWidget)
    {
        MyUserWidget->SetWCooldownPercent(Percent);
    }

    // Stop ticking when finished
    if (WCooldownRemaining <= 0.0f)
    {
        GetWorldTimerManager().ClearTimer(WCooldownTickTimer);

        if (MyUserWidget)
        {
            MyUserWidget->SetWCooldownPercent(0.0f);
        }
    }
}

void AMyPlayerController::EndWCast()
{
    bWIsCasting = false;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn || !WProjectileClass)
    {
        return;
    }

    // Interrupt any current movement (MOBA-style)
    if (AAIController* AICon = Cast<AAIController>(ControlledPawn->GetController()))
    {
        AICon->StopMovement();
    }

    // Get mouse hit
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        return;
    }

    // Snapshot pawn location
    const FVector PawnLocation = ControlledPawn->GetActorLocation();

    // Lock Z for top-down aiming
    FVector TargetLocation = Hit.ImpactPoint;
    TargetLocation.Z = PawnLocation.Z;

    const FVector Direction = (TargetLocation - PawnLocation).GetSafeNormal();

    // Spawn slightly in front of pawn
    const FVector WSpawnLocation = PawnLocation + Direction * 100.0f;
    const FRotator SpawnRotation = Direction.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = ControlledPawn;

    GetWorld()->SpawnActor<AWProjectile>(
        WProjectileClass,
        WSpawnLocation,
        SpawnRotation,
        Params
    );

    bWOnCooldown = true;

    GetWorldTimerManager().SetTimer(
        WCooldownTimer,
        this,
        &AMyPlayerController::ResetWCooldown,
        WCooldownDuration,
        false
    );
    // Start W cooldown UI
    WCooldownRemaining = WCooldownDuration;

    // Stop any existing UI tick (Q or W)
    GetWorldTimerManager().ClearTimer(QCooldownTickTimer);
    GetWorldTimerManager().ClearTimer(WCooldownTickTimer);

    // Set UI to full
    if (MyUserWidget)
    {
        MyUserWidget->SetWCooldownPercent(1.0f);
    }
}

bool AMyPlayerController::IsCasting() const
{
    return bQIsCasting || bWIsCasting;
}

