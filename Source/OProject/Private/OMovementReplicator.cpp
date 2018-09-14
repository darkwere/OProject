#include "OMovementReplicator.h"

#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

UOMovementReplicator::UOMovementReplicator(){
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	Server_NetUpdFrequency = 4.0f;
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

	if(Owner){
		if(Owner->HasAuthority()){
			Owner->NetUpdateFrequency = Server_NetUpdFrequency;
		}
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
			Client_AutonomousProxy_Tick(DeltaTime);
		}break;
		case ROLE_SimulatedProxy:{
			Client_SimulatedProxy_Tick(DeltaTime);
		}break;
	}

	if(bDrawDebugOwnerNetRole){
		DrawDebugOwnerRole();
	}

}

void UOMovementReplicator::Server_Move_Implementation(const FOMove& Move){
	if(Movement){
		Movement->SimulateMove(Move);
		UpdateServerState(Move);
	}
}

bool UOMovementReplicator::Server_Move_Validate(const FOMove& Move){
	return( !(FMath::Abs(Move.MoveInput.X) > 1) && !(FMath::Abs(Move.MoveInput.Y) > 1) );
}

void UOMovementReplicator::OnRep_ServerState(){
	if(!Movement){ return; }

	switch(GetOwnerRole()){
		case ROLE_AutonomousProxy: 	OnRep_ServerState_AutonomousProxy(); 	break;
		case ROLE_SimulatedProxy:	OnRep_ServerState_SimulatedProxy();		break;
	}
}

void UOMovementReplicator::Client_AutonomousProxy_Tick(const float DeltaTime){
	FOMove CurrentMove = Movement->GetLastMove();
	Server_Move(CurrentMove);
	if(UnacknowledgeMoves.Num() > 250){
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] Too much unacknowledged moves. Skip current"));
	}else{ 
		UnacknowledgeMoves.Add(CurrentMove); 
	}

	if(Client_LastKnownServerLocation != FVector::ZeroVector){
		FVector NewLocation = FMath::LerpStable(Movement->GetLocation(), Client_LastKnownServerLocation, DeltaTime);
		Movement->SetLocation(NewLocation);
	}

}

void UOMovementReplicator::OnRep_ServerState_AutonomousProxy(){

	FVector Client_LocalLocation = Movement->GetLocation();

	Movement->SetLocation(ServerState.Location);
	Movement->SetColliderLocation(ServerState.Location);
	Movement->SetLookAt(ServerState.LookAt);

	ClearUnacknowledgeMoves(ServerState.LastMove.Time);

	for(const FOMove& Move : UnacknowledgeMoves){
		Movement->SimulateMove(Move);
	}

	Client_LastKnownServerLocation = Movement->GetLocation();
	Movement->SetLocation(Client_LocalLocation);
}

void UOMovementReplicator::OnRep_ServerState_SimulatedProxy(){
	Client_TimeBetweenUpdates = Client_TimeSinceUpdate;
	Client_TimeSinceUpdate = 0;
	Client_CurrentLocation = Movement->GetLocation();
	Client_LastKnownServerLocation = ServerState.Location;
	
	Movement->SetLookAt(ServerState.LookAt);
	Movement->SetColliderLocation(Client_LastKnownServerLocation);
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

void UOMovementReplicator::Client_SimulatedProxy_Tick(const float DeltaTime){
	if(!Movement){ return; }

	Client_TimeSinceUpdate += DeltaTime;
	if( Client_TimeBetweenUpdates < KINDA_SMALL_NUMBER ){ return; }

	float LerpCoef = Client_TimeSinceUpdate / Client_TimeBetweenUpdates;
	FVector Client_NewLocation = FMath::LerpStable(Client_CurrentLocation, Client_LastKnownServerLocation, LerpCoef).GetSafeNormal() * Client_LastKnownServerLocation.Size();

	Movement->SetLocation(Client_NewLocation);
	Movement->SetServerStateLastMove(ServerState.LastMove);
}

void UOMovementReplicator::DrawDebugOwnerRole(){
	FColor RoleColor = FColor::White;
	switch(GetOwnerRole()){
		case ROLE_SimulatedProxy:	RoleColor = FColor::Red; 	break;
		case ROLE_AutonomousProxy:	RoleColor = FColor::Blue;	break;
		case ROLE_Authority:		RoleColor = FColor::Green;	break;
	}
	DrawDebugSphere(GetWorld(), Movement->GetLocation() * 1.2, 5, 16, RoleColor);
}

void UOMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UOMovementReplicator, ServerState);
}
