// Fill out your copyright notice in the Description page of Project Settings.

#include "OPlayerController.h"

#include "OPawn.h"

bool AOPlayerController::InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex){
    Super::InputTouch(Handle, Type, TouchLocation, Force, DeviceTimestamp, TouchpadIndex);

    if(TouchpadIndex == 0){
        AOPawn* Pawn = Cast<AOPawn>(GetPawn());
        if(Pawn){
            Pawn->HandleTouch(Type, TouchLocation);
            return true;
        }        
    }
    return false;
}