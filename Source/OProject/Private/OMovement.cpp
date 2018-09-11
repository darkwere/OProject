#include "OMovement.h"


UOMovement::UOMovement(){	
	PrimaryComponentTick.bCanEverTick = true;
}

void UOMovement::BeginPlay(){
	Super::BeginPlay();
}


void UOMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!Collider || !SkeletalMesh){
		UE_LOG(LogTemp, Error, TEXT("[ERROR] UOMovement -> Root Collider or Skeletal Mesh didn't initilize"));
		return;
	}

	FOMove NewMove = CreateMove(DeltaTime);

	SimulateMove(NewMove);

}

void UOMovement::Init(USceneComponent* Collider, USceneComponent* SkeletalMesh){
	this->Collider = Collider;
	this->SkeletalMesh = SkeletalMesh;
}

void UOMovement::MoveForward(const float Value){
	MoveInput.X = AdjustMoveInput(Value);
}
	
void UOMovement::MoveRight(const float Value){
	MoveInput.Y = AdjustMoveInput(Value);
}
	
void UOMovement::RotateDelta(const float DeltaRotation){

}
	
void UOMovement::SimulateMove(const FOMove& Move){
	MoveAround(Move);
	LookAtRotation(Move);
	AdjustMeshUp();
}


void UOMovement::SetLocation(const FVector& NewLocation){
	if(SkeletalMesh){
		SkeletalMesh->SetWorldLocation(NewLocation);
		AdjustMeshUp();
	}
}

void UOMovement::SetColliderLocation(const FVector& NewLocation){
	if(Collider){
		Collider->SetWorldLocation(NewLocation);
	}
}

void UOMovement::SetLookAt(const FRotator& NewLookAt){
	if(SkeletalMesh){
		SkeletalMesh->SetRelativeRotation(NewLookAt);
	}
}

// PRIVATE

float UOMovement::AdjustMoveInput(const float Value) const{
	float RetValue = 0.0f;
	if(FMath::Abs(Value) > 0.1f){
		RetValue = -FMath::Sign(Value);
	}
	return(RetValue);
}

FOMove UOMovement::CreateMove(const float DeltaTime) const{
	FOMove NewMove;
	NewMove.DeltaTime = DeltaTime;
	NewMove.MoveInput = MoveInput;
	NewMove.DeltaRotation = DeltaRotaion;
	NewMove.Time = GetWorld()->TimeSeconds;
	return(NewMove);
}

void UOMovement::MoveAround(const FOMove& Move){
	if (!SkeletalMesh || !Collider) {
		UE_LOG(LogTemp, Error, TEXT("Unable to move -> root componetn is not initialized!"));
		return;
	}

	FVector MeshForward = SkeletalMesh->GetForwardVector();
	FVector MeshRight = SkeletalMesh->GetRightVector();
	FVector Location = SkeletalMesh->GetComponentLocation();


	float Speed = PawnSpeed * Move.DeltaTime;

	FVector RotateAround = MeshForward * Move.MoveInput.X + MeshRight * Move.MoveInput.Y;

	if (RotateAround == FVector::ZeroVector) {
		return;
	}

	FQuat Quaternion(RotateAround, FMath::DegreesToRadians(PawnSpeed));
	Location = Quaternion.RotateVector(Location);
	FHitResult Hit;
	Collider->SetWorldLocation(Location, true, &Hit);
	if (Hit.IsValidBlockingHit()) {
		RotateAround = FVector::ZeroVector;
	}else {
		SkeletalMesh->SetWorldLocation(Location);
	}
}
	
void UOMovement::LookAtRotation(const FOMove& Move){
	if(!SkeletalMesh){
		return;
	}

	if(FMath::Abs(Move.DeltaRotation) < KINDA_SMALL_NUMBER) { return; }

	FQuat Qt(FVector(0, 0, 1), FMath::DegreesToRadians(Move.DeltaRotation));
	SkeletalMesh->AddRelativeRotation(Qt);
}
	
void UOMovement::AdjustMeshUp(){
	if(!SkeletalMesh){ return; }

	FVector Up = SkeletalMesh->GetUpVector();
	FVector MeshLocation = SkeletalMesh->GetComponentLocation();
	FVector AimUpVector = MeshLocation.GetSafeNormal();
	FQuat RotQuat = FQuat::FindBetweenNormals(Up, AimUpVector);
	SkeletalMesh->AddWorldRotation(RotQuat);
}