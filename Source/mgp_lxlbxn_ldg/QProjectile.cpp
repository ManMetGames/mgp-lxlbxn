#include "QProjectile.h"
#include "DamageDummy.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

AQProjectile::AQProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    // Sphere as hitbox
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(15.0f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->SetGenerateOverlapEvents(false);
    SetRootComponent(Collision);

    // Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Hit event
    Collision->OnComponentHit.AddDynamic(this, &AQProjectile::OnProjectileHit);
}

void AQProjectile::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(LifeTime);
}

void AQProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector NewLocation = GetActorLocation() + GetActorForwardVector() * Speed * DeltaTime;
    // Constant Z
    NewLocation.Z = GetActorLocation().Z;
    SetActorLocation(NewLocation, true);
}

void AQProjectile::OnProjectileHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    if (ADamageDummy* Dummy = Cast<ADamageDummy>(OtherActor))
    {
        if (Dummy->HasAbilityMark)
        {
            const float BonusDamage = 40.0f;

            UGameplayStatics::ApplyDamage(
                Dummy,
                BonusDamage,
                GetInstigatorController(),
                this,
                UDamageType::StaticClass()
            );

            Dummy->ConsumeAbilityMark();
        }
    }
    

    if (OtherActor && OtherActor != this)
    {
        if (GEngine)
        {   
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::Red,
                FString::Printf(TEXT("Projectile hit: %s"), *OtherActor->GetName())
            );
        }

        Destroy();
    }
}