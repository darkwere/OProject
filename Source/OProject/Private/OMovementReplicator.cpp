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

	switch(GetOwnerRole()){
		case ROLE_Authority:{
			if(GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy){
				FOMove CurrentMove = Movement->GetLastMove();
				UpdateServerState(CurrentMove);
			}
		}break;
		case ROLE_AutonomousProxy:{
			FOMove CurrentMove = Movement->GetLastMove();
			Server_Move(CurrentMove);
			if(UnacknowledgeMoves.Num() > 250){ 
				UE_LOG(LogTemp, Warning, TEXT("[ERROR] Too much unacknowledged moves. Skip current"));
			}else{ UnacknowledgeMoves.Add(CurrentMove); }
		}break;
		case ROLE_SimulatedProxy:{
			Client_Tick(DeltaTime);
		}break;
	}
}

void UOMovementReplicator::Server_Move_Implementation(const FOMove& Move){
	if(Movement){
		Movement->SimulateMove(Move);
		UpdateServerState(Move);
	}
}

bool UOMovementReplicator::Server_Move_Validate(const FOMove& Move){
	return(true);
}

void UOMovementReplicator::OnRep_ServerState(){
	if(!Movement){ return; }



	ENetRole OwnerRole = GetOwnerRole();

	switch(OwnerRole){

		case ROLE_AutonomousProxy:{
			
			Movement->SetLocation(ServerState.Location);
			Movement->SetColliderLocation(ServerState.Location);
			Movement->SetLookAt(ServerState.LookAt);

			ClearUnacknowledgeMoves(ServerState.LastMove.Time);

			for(const FOMove& Move : UnacknowledgeMoves){
				Movement->SimulateMove(Move);
			}
		}
		break;

		case ROLE_SimulatedProxy:{
			Client_TimeBetweenUpdates = Client_TimeSinceUpdate;
			Client_TimeSinceUpdate = 0;
			Client_CurrentLocation = Movement->GetLocation();

			Movement->SetLookAt(ServerState.LookAt);
			Client_LastKnownServerLocation = ServerState.Location;

			Movement->SetColliderLocation(Client_LastKnownServerLocation);
		}

		break;
	}

}

void UOMovementReplicator::UpdateServerState(const FOMove& Move){
	if(Movement){
		ServerState.Location	= Movement->GetLocation();
		ServerState.LookAt 		= Movement->GetLookAt();
		ServerState.LastMove 	= Move;
	}
}

void UOMovementReplicator::ClearUnacknowledgeMoves(const float Time){
	int size = UnacknowledgeMoves.Num();
	TArray<FOMove> NewMoves;
	for(const FOMove& Move : UnacknowledgeMoves){
		if(Move.Time > Time){
			NewMoves.Add(Move);
		}
	}
	UnacknowledgeMoves = NewMoves;
}

void UOMovementReplicator::Client_Tick(const float DeltaTime){
	if(!Movement){ return; }

	Client_TimeSinceUpdate += DeltaTime;
	if( Client_TimeSinceUpdate < KINDA_SMALL_NUMBER ){ return; }

	float LerpCoef = Client_TimeSinceUpdate / Client_TimeBetweenUpdates;
	FVector Client_NewLocation = FMath::LerpStable(Client_CurrentLocation, Client_LastKnownServerLocation, LerpCoef).GetSafeNormal() * Client_LastKnownServerLocation.Size();

	Movement->SetLocation(Client_NewLocation);
	// TODO: Update LastMove for proper Animations
}

void UOMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UOMovementReplicator, ServerState);
}
