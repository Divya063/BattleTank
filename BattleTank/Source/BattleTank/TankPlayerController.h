
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

class ATank;
class UTankAimingComponent;

/**
 * 
 */
UCLASS()
class BATTLETANK_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float CrossHairXLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrossHairYLocation = 0.33333333;

	UPROPERTY(EditDefaultsOnly)
	int32 LineTraceRange = 10000000;

	bool GetSightRayHitLocation(FVector& OutHitLocation) const;

	bool GetLookDirection(const FVector2D &ScreenLocation, FVector& OutLookDirection) const;

	bool GetLookVectorHitLocation(const FVector LookDirection, FVector& OutHitLocation) const;

	// Start moving the barrel so that a shot would hit where the crosshair intersects the world.
	void AimTowardsCrosshair();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Setup")
	void FoundAimingComponent(UTankAimingComponent* AimCompRef);

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetPawn(APawn* InPawn) override;

private:
	UFUNCTION()
	void OnTankDeath();
};
