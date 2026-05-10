/**
*   What im trying to do here -
*   Handle input with movement Q and W
*   set Casting and cooldown times
*   and hook them up with the UI to show cooldowns
*   trying to follow the style of mobas with my main inspo being from league (my main game)
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "QProjectile.h"
#include "Components/DecalComponent.h"
#include "MyPlayerController.generated.h"

class UMyUserWidget;
class AWProjectile;
class UDecalComponent;

UCLASS()
class MGP_LXLBXN_LDG_API AMyPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;
    //Movement
    void OnMoveCommand();
    void StopCurrentMovement();
    virtual void Tick(float DeltaSeconds) override;
    // Ability Functions
    void OnQPressed(); 
    void ResetQCooldown();
    void EndQCast();
    void TickQCooldown();
    void ResetWCooldown();

    void OnWPressed(); 
    void EndWCast();
    void TickWCooldown();
    bool IsCasting() const;
    // Ability states
    bool bQOnCooldown = false;
    bool bQIsCasting = false;
    bool bWOnCooldown = false;
    bool bWIsCasting = false;


    // Location of mouse so it can be aimed
    bool GetMouseWorldLocation(FVector& OutLocation) const;

    // Cast and cd durations
    float QCastDuration = 0.2f;
    float QCooldownDuration = 1.0f;
    float QCooldownRemaining = 0.0f;

    float WCastDuration = 0.2f;
    float WCooldownDuration = 2.0f;
    float WCooldownRemaining = 0.0f;

    // Timers for cast and cd 
    FTimerHandle QCastTimer;
    FTimerHandle QCooldownTimer;
    FTimerHandle QCooldownTickTimer;

    FTimerHandle WCastTimer;
    FTimerHandle WCooldownTimer;
    FTimerHandle WCooldownTickTimer;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMyUserWidget> MyUserWidgetClass;

    UPROPERTY()
    UMyUserWidget* MyUserWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TSubclassOf<AQProjectile> QProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TSubclassOf<AWProjectile> WProjectileClass;

};