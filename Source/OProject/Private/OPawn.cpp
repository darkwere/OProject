#include "OPawn.h"

#include "OMovement.h"

AOPawn::AOPawn(){
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(false);
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

void AOPawn::HandleTouch(ETouchType::Type Type, const FVector2D& TouchLocation){
	if(!Movement){ return; }

	switch(Type){
		case ETouchType::Began:
			PrevTouchLocation = TouchLocation;
		break;
		case ETouchType::Moved:{
			float DeltaRotation = (TouchLocation.X - PrevTouchLocation.X);
			Movement->RotateDelta(DeltaRotation);
			PrevTouchLocation = TouchLocation;
		}
		break;
		case ETouchType::Stationary:
			Movement->RotateDelta(0);
		break;
		case ETouchType::Ended:
			PrevTouchLocation = FVector2D::ZeroVector;
			Movement->RotateDelta(0);
		break;
		case ETouchType::NumTypes:
		break;
		default:
			PrevTouchLocation = FVector2D::ZeroVector;
		break;
	}
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
