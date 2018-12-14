
#include "Public/TankAimingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Public/TankBarrel.h"
#include "TankTurret.h"
#include "Engine/World.h"
#include "Projectile.h"

// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...   
}

void UTankAimingComponent::BeginPlay()
{
	// ensure all players must wait for reload time before starting to fire
	LastFireTime = FPlatformTime::Seconds();
}

void UTankAimingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (Ammo == 0)
	{
		FiringStatus = EFiringStatus::OutOfAmmo;
	}
	else if (FPlatformTime::Seconds() - LastFireTime < ReloadTimeInSeconds)
	{
		FiringStatus = EFiringStatus::Reloading;
		//UE_LOG(LogTemp, Warning, TEXT("Reloading"))
	}
	else if (IsBarrelMoving())
	{
		FiringStatus = EFiringStatus::Aiming;
		//UE_LOG(LogTemp, Warning, TEXT("Aiming"))
	}
	else
	{
		FiringStatus = EFiringStatus::Locked;
		//UE_LOG(LogTemp, Warning, TEXT("Locked"))
	}
}

bool UTankAimingComponent::IsBarrelMoving()
{
	if (!ensure(Barrel)) { return false; }
	auto BarrelForwardVector = Barrel->GetForwardVector();
	return !BarrelForwardVector.GetSafeNormal().Equals(CurrentAimDirection.GetSafeNormal(), 0.01);
}


void UTankAimingComponent::Initialise(UTankBarrel * BarrelToSet, UTankTurret * TurretToSet)
{
	Barrel = BarrelToSet;
	Turret = TurretToSet;
}

void UTankAimingComponent::AimAt(const FVector& HitLocation)
{
	if (!ensure(Barrel && Turret)) { return; }
	FVector LaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));

	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		LaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (bHaveAimSolution)
	{
		auto TankName = GetOwner()->GetName();
		CurrentAimDirection = LaunchVelocity.GetSafeNormal();
		MoveBarrelTowards(CurrentAimDirection);
	}
}

void UTankAimingComponent::MoveBarrelTowards(const FVector& AimDirection)
{
	if (!ensure(Barrel && Turret)) { return; }
	auto BarrelRotator = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = AimDirection.Rotation();
	auto DeltaRotator = AimAsRotator - BarrelRotator;
	Barrel->Elevate(DeltaRotator.Pitch);
	if (FMath::Abs(DeltaRotator.Yaw) < 180.0) 
	{
		Turret->Rotate(DeltaRotator.Yaw);
	}
	else
	{
		Turret->Rotate(-DeltaRotator.Yaw);
	}
	
}

void UTankAimingComponent::Fire()
{
	if (FiringStatus == EFiringStatus::Aiming || FiringStatus == EFiringStatus::Locked) {
		if (!ensure(Barrel)) { return; }
		if (!ensure(ProjectileBlueprint)) { return; }
		FVector ProjectileLocation = Barrel->GetSocketLocation(FName("Projectile"));
		FRotator ProjectileRotation = Barrel->GetSocketRotation(FName("Projectile"));
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileBlueprint, ProjectileLocation, ProjectileRotation);
		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
		Ammo--;
	}
}


EFiringStatus UTankAimingComponent::GetFiringStatus() const
{
	return FiringStatus;
}

int32 UTankAimingComponent::GetCurrentAmmo() const
{
	return Ammo;
}