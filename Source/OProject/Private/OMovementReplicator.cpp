#include "OMovementReplicator.h"

#include "Net/UnrealNetwork.h"

UOMovementReplicator::UOMovementReplicator(){
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

void UOMovementReplicator::BeginPlay(){
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if(Owner){
		Movement = Cast<UOMovement>(Owner->GetComponentByClass(UOMovement::StaticClass()));
	}

	if(!Movement){
		UE_LOG(LogTemp, Error, TEXT("[ERROR] Unable to find UOMovement Component for the owner! Replication won't work."));
	}

}

void UOMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(!Movement){ return; }

	ENetRole OwnerRole = GetOwnerRole();
	ENetRole RemoteRole = GetOwner()->GetRemoteRole();

	if(OwnerRole == ROLE_AutonomousProxy){
		FOMove CurrentMove = Movement->GetLastMove();
		Server_Move(CurrentMove);
	}

	if(OwnerRole == ROLE_Authority && RemoteRole == ROLE_SimulatedProxy){
		FOMove CurrentMove = Movement->GetLastMove();
		UpdateServerState(CurrentMove);
	}

	// TODO: Replicate to Simulated proxy?

}

void UOMovementReplicator::Server_Move_Implementation(const FOMove& Move){
	if(Movement){
		Movement->SimulateMove(Move);
		UpdateServerState(Move);
	}
}

bool UOMovementReplicator::Server_Move_Validate(const FOMove& Move){
	return true;
}

void UOMovementReplicator::OnRep_ServerState(){
	if(Movement){
		Movement->SetLocation(ServerState.Location);
		Movement->SetColliderLocation(ServerState.Location);
		Movement->SetLookAt(ServerState.LookAt);
	}

}

void UOMovementReplicator::UpdateServerState(const FOMove& Move){
	if(Movement){
		ServerState.Location	= Movement->GetLocation();
		ServerState.LookAt 		= Movement->GetLookAt();
		ServerState.LastMove 	= Move;
	}
}

void UOMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UOMovementReplicator, ServerState);
}
