#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "OMovement.h"

#include "OMovementReplicator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPROJECT_API UOMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOMovementReplicator();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Move(const FOMove& Move);

	UFUNCTION()
	void OnRep_ServerState();

	void UpdateServerState(const FOMove& Move);
	void ClearUnacknowledgeMoves(const float Time);
	void Client_Tick(const float DeltaTime);

// FIELDS

private:

	UOMovement* Movement;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FOState ServerState;

	// ROLE_AutonomousProxy related
	TArray<FOMove> UnacknowledgeMoves;

	// ROLE_SimulatedProxy Related
	float Client_TimeBetweenUpdates;
	float Client_TimeSinceUpdate;

	FVector Client_CurrentLocation;
	FVector Client_LastKnownServerLocation;



};
