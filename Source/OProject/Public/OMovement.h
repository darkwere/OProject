#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OMovement.generated.h"


USTRUCT()
struct FOMove{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector2D MoveInput;
	UPROPERTY()
	float DeltaRotation;
	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	float Time;
};

USTRUCT()
struct FOState{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator LookAt;
	UPROPERTY()
	FOMove LastMove;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPROJECT_API UOMovement : public UActorComponent{
	GENERATED_BODY()

public:	
	UOMovement();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Init")
	void Init(USceneComponent* Collider, USceneComponent* SkeletalMesh);

	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void RotateDelta(const float DeltaRotation);
	
	void SimulateMove(const FOMove& Move);
		
	void SetLocation(const FVector& NewLocation);
	void SetColliderLocation(const FVector& NewLocation);
	void SetLookAt(const FRotator& NewLookAt);

	FOMove 	GetLastMove() const;
	FVector	GetLocation() const;
	FRotator GetLookAt() const;

protected:

	virtual void BeginPlay() override;

private:

	float AdjustMoveInput(const float Value) const;

	FOMove CreateMove(const float DeltaTime) const;

	void MoveAround(const FOMove& Move);
	void LookAtRotation(const FOMove& Move);
	void AdjustMeshUp();

// FIELDS

protected:

	UPROPERTY(EditDefaultsOnly)
	float PawnSpeed;

	USceneComponent* Collider;
	USceneComponent* SkeletalMesh;

	FVector2D MoveInput;
	float DeltaRotation;
	FOMove	LastMove;
};
