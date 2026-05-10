#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "QProjectile.generated.h"

/**
* the damage skillshot
* do damage if hit
* consume mark if hits someone marked
*/
UCLASS()
class MGP_LXLBXN_LDG_API AQProjectile : public AActor
{
    GENERATED_BODY()

public:
    AQProjectile();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

protected:
    // Collision
    UPROPERTY(VisibleAnywhere)
    USphereComponent* Collision;

    // Visual
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    // Movement
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float Speed = 600.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float LifeTime = 3.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float Damage = 50.0f;

    // Collision
    UFUNCTION()
    void OnProjectileHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

};