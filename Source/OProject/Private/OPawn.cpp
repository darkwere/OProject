#include "OPawn.h"

#include "OMovement.h"

AOPawn::AOPawn(){
	PrimaryActorTick.bCanEverTick = true;

}

void AOPawn::BeginPlay(){
	Super::BeginPlay();
	Movement = Cast<UOMovement>(GetComponentByClass(UOMovement::StaticClass()));
}

void AOPawn::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}


void AOPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AOPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AOPawn::MoveRight);
}


void AOPawn::MoveForward(const float Value){
	if(Movement){
		Movement->MoveForward(Value);
	}
}


void AOPawn::MoveRight(const float Value){
	if(Movement){
		Movement->MoveRight(Value);
	}
}
