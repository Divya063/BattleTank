

#include "TankPlayerController.h"
#include "Engine/World.h"
#include "TankAimingComponent.h"
#include "Tank.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!GetPawn()) { return; } // if not possessing
	
	auto TankAimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(TankAimingComponent)) { return; }
	
	FoundAimingComponent(TankAimingComponent); 
}

void ATankPlayerController::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}

void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetPawn()) { return; } // if not possessing
	auto TankAimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(TankAimingComponent)) { return; }
	
	FVector HitLocation;
	bool bGotHitLocation = GetSightRayHitLocation(HitLocation);
	if (bGotHitLocation)
	{
		TankAimingComponent->AimAt(HitLocation);
	}
}

// Get location of line trace through crosshair, true if hits landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector& OutHitLocation) const
{
	int32 ViewportSizeX, ViewportSizeY;	
	GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D CrossHairScreenLocation(ViewportSizeX * CrossHairXLocation, ViewportSizeY * CrossHairYLocation);

	FVector LookDirection;
	if (GetLookDirection(CrossHairScreenLocation, LookDirection))
	{
		return GetLookVectorHitLocation(LookDirection, OutHitLocation);
	}

	// no direction or no hit
	return false;
}

bool ATankPlayerController::GetLookDirection(const FVector2D &ScreenLocation, FVector& OutLookDirection) const
{
	FVector LookDirection;
	FVector CameraWorldLocation;
	return DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, CameraWorldLocation, OutLookDirection);
}

bool ATankPlayerController::GetLookVectorHitLocation(const FVector LookDirection, FVector& OutHitLocation) const
{
	// Use Player location as start point
	FVector LineTraceStart = PlayerCameraManager->GetCameraLocation(); 

	// End point is location + direction * distance
	FVector LineTraceEnd = LineTraceStart + LookDirection * LineTraceRange;

	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	if(GetWorld()->LineTraceSingleByChannel(
			HitResult,
			LineTraceStart,
			LineTraceEnd,
			ECollisionChannel::ECC_Camera)
		)
	{
		OutHitLocation = HitResult.Location;
		return true;
	}
	// no hit
	OutHitLocation = FVector(0);
	return false;
}


void ATankPlayerController::SetPawn(APawn *InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossesedTank = Cast<ATank>(InPawn);
		if (!ensure(PossesedTank)) { return; }
		PossesedTank->OnTankDeath.AddUniqueDynamic(this, &ATankPlayerController::OnTankDeath);
	}
}

void ATankPlayerController::OnTankDeath()
{
	StartSpectatingOnly();
	UE_LOG(LogTemp, Warning, TEXT("Player Tank has dies. Player now spectating only!"))
}