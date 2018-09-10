#include "OMovement.h"


UOMovement::UOMovement(){	
	PrimaryComponentTick.bCanEverTick = true;
}

void UOMovement::BeginPlay(){
	Super::BeginPlay();
	// TODO: Init Collider and Skeletal Mesh in BP
}


void UOMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOMovement::MoveForward(const float Value){

}
	
void UOMovement::MoveRight(const float Value){
	
}
	
void UOMovement::RotateDelta(const float DeltaRotation){

}
	
void UOMovement::SimulateMove(const FOMove& Move){

}


void UOMovement::SetLocation(const FVector& NewLocation){

}

void UOMovement::SetColliderLocation(const FVector& NewLocation){

}

void UOMovement::SetLookAt(const FRotator& NewLookAt){

}

// PRIVATE

float UOMovement::AdjustMoveInput(const float Value) const{
	return 0.0f;
}

FOMove UOMovement::CreateMove(const float DeltaTime) const{
	return FOMove();
}

void UOMovement::MoveAround(const FOMove& Move){

}
	
void UOMovement::LookAtRotation(const FOMove& Move){

}
	
void UOMovement::AdjustMeshUp(){

}