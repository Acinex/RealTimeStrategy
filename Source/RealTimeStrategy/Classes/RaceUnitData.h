#pragma once

#include "RaceUnitData.generated.h"

USTRUCT(BlueprintType)
struct FRaceUnitData
{
	GENERATED_BODY()

	/** Actors to spawn for each player in the game. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	TArray<TSubclassOf<AActor>> InitialActors;

	/** Relative locations of the actors to spawn for each player in the game, relative to their respective start spot. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	TArray<FVector> InitialActorLocations;

	/** Optional types of actors that are required for a player to be alive. As soon as no actor of the specified type is alive, the player is defeated. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	TArray<TSubclassOf<AActor>> DefeatConditionActorClasses;

	/** Units and buildings the AI should produce, in order. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	TArray<TSubclassOf<AActor>> BuildOrder;

	UPROPERTY(EditDefaultsOnly, Category="RTS")
	bool bIncludeInitialActorsInBuildOrder = true;

	TArray<TSubclassOf<AActor>> GetBuildOrder() const;
};
