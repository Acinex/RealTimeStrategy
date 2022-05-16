#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerStart.h"

#include "RTSPlayerStart.generated.h"


/**
* Player start with RTS features, such as which player has been spawned at this start.
*/
UCLASS(Category="RTS")
class REALTIMESTRATEGY_API ARTSPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	/** Gets the team to add the spawned player to. */
	UFUNCTION(BlueprintPure)
	int32 GetTeamIndex() const;

	/** Gets the player who has been spawned at this start. */
	UFUNCTION(BlueprintPure)
	AController* GetPlayer() const;

	/** Sets the player who's been spawned at this start. */
	void SetPlayer(AController* InPlayer);

	FRotator GetControllerRotation() const;

protected:
	UPROPERTY(EditAnywhere, Category="RTS")
	bool bUseActorRotationForController = true;

	UPROPERTY(EditAnywhere, meta=(EditCondition="!bUseActorRotationForController"), Category="RTS")
	FRotator ControllerRotation = FRotator::ZeroRotator;

private:
	/** Team to add the spawned player to. */
	UPROPERTY(EditInstanceOnly, Category = "RTS|Team")
	uint8 TeamIndex;

	/** Player who's been spawned at this start. */
	UPROPERTY()
	AController* Player;
};
