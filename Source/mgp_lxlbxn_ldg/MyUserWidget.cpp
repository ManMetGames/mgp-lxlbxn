#include "MyUserWidget.h"
#include "Engine/Engine.h"
#include "Components/Image.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

//Creates independent mat instances for q and w 
void UMyUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Create a dynamic material instance from the Image's material

    if (QIconImage)
    {
        UMaterialInterface* BaseMat =
            Cast<UMaterialInterface>(QIconImage->GetBrush().GetResourceObject());

        if (BaseMat)
        {
            QCooldownMID = UMaterialInstanceDynamic::Create(BaseMat, this);
            QIconImage->SetBrushFromMaterial(QCooldownMID);
            QCooldownMID->SetScalarParameterValue(TEXT("Cooldown"), 0.0f); 
        }
    }

    if (WIconImage)
    {
        UMaterialInterface* BaseMat =
            Cast<UMaterialInterface>(WIconImage->GetBrush().GetResourceObject());
            
        if (BaseMat)
        {
            WCooldownMID = UMaterialInstanceDynamic::Create(BaseMat, this);
            WIconImage->SetBrushFromMaterial(WCooldownMID);
            WCooldownMID->SetScalarParameterValue(TEXT("Cooldown"), 0.0f);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("QIcon ptr: %p"), QIconImage);
    UE_LOG(LogTemp, Warning, TEXT("WIcon ptr: %p"), WIconImage);
    // Initialize to full (visible)

}

void UMyUserWidget::SetQCooldownPercent(float Percent)
{
    if (!QCooldownMID)
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("SET Q UI: %f"), Percent);

    if (QCooldownMID)
    {
        QCooldownMID->SetScalarParameterValue(TEXT("Cooldown"), Percent);
    }
}

void UMyUserWidget::SetWCooldownPercent(float Percent)
{
    UE_LOG(LogTemp, Warning, TEXT("SET W UI: %f"), Percent);
    if (WCooldownMID)
    {
        WCooldownMID->SetScalarParameterValue(TEXT("Cooldown"), Percent);
    }
}
