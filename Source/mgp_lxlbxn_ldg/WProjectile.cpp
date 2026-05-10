
#include "WProjectile.h"
#include "DamageDummy.h"

AWProjectile::AWProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(20.0f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    SetRootComponent(Collision);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Collision->OnComponentHit.AddDynamic(this, &AWProjectile::OnHit);
}

void AWProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AWProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector NewLocation = GetActorLocation() + GetActorForwardVector() * Speed * DeltaTime;
    NewLocation.Z = GetActorLocation().Z;
    SetActorLocation(NewLocation, true);
}

void AWProjectile::OnHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    if (ADamageDummy* Dummy = Cast<ADamageDummy>(OtherActor))
    {
        Dummy->ApplyAbilityMark();
    }

    Destroy();
}
