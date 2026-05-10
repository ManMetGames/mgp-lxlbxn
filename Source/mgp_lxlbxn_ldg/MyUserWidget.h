// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Materials/MaterialInstanceDynamic.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MyUserWidget.generated.h"

/**
* My goal for this one is for the visuals of the cooldowns
* so we can distinguish which ones usable and not
* using their own image widget and dyn material instance
*/

UCLASS()
class MGP_LXLBXN_LDG_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    // normalized so 0 = ready and vice versa)
    void SetQCooldownPercent(float Percent);
    void SetWCooldownPercent(float Percent);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UImage* QIconImage;

    UPROPERTY()
    UMaterialInstanceDynamic* QCooldownMID;

    UPROPERTY(meta = (BindWidget))
    UImage* WIconImage;

    UPROPERTY()
    UMaterialInstanceDynamic* WCooldownMID;
};
