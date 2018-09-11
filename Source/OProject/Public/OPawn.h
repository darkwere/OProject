#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OPawn.generated.h"

UCLASS()
class OPROJECT_API AOPawn : public APawn{
	GENERATED_BODY()

public:
	AOPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleTouch(ETouchType::Type Type, const FVector2D& TouchLocation);

protected:
    
	virtual void BeginPlay() override;

	void MoveForward(const float Value);
	void MoveRight(const float Value);



// FIELDS

public:

protected:

	UPROPERTY(VisibleAnywhere, Category = "OPawn")
	class UOMovement* Movement;

private:

	FVector2D PrevTouchLocation;



};
