// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OPROJECT_API AOPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	virtual bool InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override;
	
	
	
};
