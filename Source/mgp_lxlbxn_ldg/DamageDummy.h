#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageDummy.generated.h"

/**
* prototype for damage testing
* could be replaced later 
*/

UCLASS()
class MGP_LXLBXN_LDG_API ADamageDummy : public AActor
{
    GENERATED_BODY()

public:
    ADamageDummy();

    UPROPERTY(VisibleAnywhere, Category = "Status")
    bool HasAbilityMark = false;

    void ApplyAbilityMark();
    void ConsumeAbilityMark();

protected:
    //So in the bp the mesh for dummy wont be attached to the w ability mesh
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHealth;

    virtual void BeginPlay() override;

    // Damage receiver
    UFUNCTION()
    void HandleDamage(
        AActor* DamagedActor,
        float Damage,
        const UDamageType* DamageType,
        AController* InstigatorControllor,
        AActor* DamageCauser
    );

    UPROPERTY(VisibleAnywhere, Category = "Visual")
    UStaticMeshComponent* WMarkVisual;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* StaticMesh;
};