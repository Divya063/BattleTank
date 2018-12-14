
#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "TankAIController.generated.h"

class UTankAimingComponent;
/**
 * 
 */
UCLASS()
class BATTLETANK_API ATankAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void SetPawn(APawn* InPawn) override;

protected:
	// How close AI tank can get to player
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float AcceptanceRadius = 8000;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float RangeOfSight = 12500;
private:
	UFUNCTION()
	void OnTankDeath();

	virtual void Tick(float DeltaTime) override;
};
