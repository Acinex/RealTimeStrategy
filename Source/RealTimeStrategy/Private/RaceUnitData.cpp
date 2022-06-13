#include "RaceUnitData.h"

TArray<TSubclassOf<AActor>> FRaceUnitData::GetBuildOrder() const
{
	TArray<TSubclassOf<AActor>> Classes;

	if (bIncludeInitialActorsInBuildOrder)
	{
		Classes.Append(InitialActors);
	}

	Classes.Append(BuildOrder);

	return Classes;
}
