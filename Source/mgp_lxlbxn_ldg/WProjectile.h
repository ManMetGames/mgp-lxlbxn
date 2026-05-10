
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "WProjectile.generated.h"

UCLASS()
class MGP_LXLBXN_LDG_API AWProjectile : public AActor
{
    GENERATED_BODY()

public:
    AWProjectile();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere)
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float Speed = 700.0f;

    UFUNCTION()
    void OnHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );
};
