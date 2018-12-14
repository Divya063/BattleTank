
#include "Public/TankAIController.h"
#include "TankPlayerController.h"
#include "Engine/World.h"
#include "TankAimingComponent.h"
#include "Tank.h"
#include "Components/StaticMeshComponent.h"

void ATankAIController::Tick(float DeltaTime)
{
	auto PlayerTank = GetWorld()->GetFirstPlayerController()->GetPawn();
	auto ControlledTank = GetPawn();
	if (!PlayerTank || !ControlledTank) { return; }
	
	MoveToActor(PlayerTank, AcceptanceRadius); // TODO check radius in cm

	auto PlayerLocation = PlayerTank->GetActorLocation();
	auto ControlledTankLocation = ControlledTank->GetActorLocation();
	auto DirectionToPlayer = (ControlledTankLocation + PlayerLocation).GetSafeNormal();
	auto DistanceToPlayer = (PlayerLocation - ControlledTankLocation).Size();

	if (DistanceToPlayer > RangeOfSight)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player out of sight range %f"), DistanceToPlayer);
		return;
	}

	FHitResult HitResult;
	
	FVector TurretLocation = ControlledTank->FindComponentByClass<UStaticMeshComponent>()->GetSocketLocation(FName("Turret"));

	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	auto LineTraceResult = GetWorld()->LineTraceSingleByChannel(HitResult, TurretLocation, PlayerLocation, ECollisionChannel::ECC_Camera);

	if (!LineTraceResult)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Line Trace Failed"));
		return;
	}

	if (!(HitResult.GetActor()->GetName() == ControlledTank->GetName()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("PlayerHidden"));
		return;
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Firing!"));

	auto TankAimingComponent = ControlledTank->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(TankAimingComponent)) { return; }

	TankAimingComponent->AimAt(PlayerTank->GetActorLocation());

	if (TankAimingComponent->GetFiringStatus() == EFiringStatus::Locked)
	{
		TankAimingComponent->Fire();
	}
}

void ATankAIController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossesedTank = Cast<ATank>(InPawn);
		if (!ensure(PossesedTank)) { return; }
		PossesedTank->OnTankDeath.AddUniqueDynamic(this, &ATankAIController::OnTankDeath);
	}
}

void ATankAIController::OnTankDeath()
{
	auto Pawn = GetPawn();
	if (Pawn)
	{
		Pawn->DetachFromControllerPendingDestroy();
	}
	UE_LOG(LogTemp, Warning, TEXT("AI Tank has died."))
}