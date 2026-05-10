#include "DamageDummy.h"
#include "Engine/Engine.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"


ADamageDummy::ADamageDummy()
{
    PrimaryActorTick.bCanEverTick = false;
    // Acts as like a capsule so the w mark mesh wont affect the dummy mesh
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Attach to root so its not attached to the mark mesh as default
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(Root);

    // Create visual indicator
    WMarkVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WMarkVisual"));
    WMarkVisual->SetupAttachment(RootComponent);

    // No collision, start hidden, 
    WMarkVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WMarkVisual->SetHiddenInGame(true);
    WMarkVisual->SetMobility(EComponentMobility::Movable);

    // Prevent transform inheritance , keeps w mark on the dummy
    WMarkVisual->SetUsingAbsoluteLocation(false);
    WMarkVisual->SetUsingAbsoluteRotation(false);
    WMarkVisual->SetUsingAbsoluteScale(true);

}

void ADamageDummy::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    // Bind to damage event
    OnTakeAnyDamage.AddDynamic(this, &ADamageDummy::HandleDamage);
}

void ADamageDummy::HandleDamage(
    AActor* DamagedActor,
    float Damage,
    const UDamageType* DamageType,
    AController* InstigatorControllor,
    AActor* DamageCauser
)
{
    CurrentHealth -= Damage;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Yellow,
            FString::Printf(TEXT("Dummy took %.0f damage. HP: %.0f"), Damage, CurrentHealth)
        );
    }

    if (CurrentHealth <= 0.0f)
    {
        Destroy();
    }
}

void ADamageDummy::ApplyAbilityMark()
{
    HasAbilityMark = true;
    WMarkVisual->SetHiddenInGame(false);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Cyan,
            TEXT("Ability mark applied")
        );
    }
}

void ADamageDummy::ConsumeAbilityMark()
{
    HasAbilityMark = false;
    WMarkVisual->SetHiddenInGame(true);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Magenta,
            TEXT("Ability mark consumed")
        );
    }
}
