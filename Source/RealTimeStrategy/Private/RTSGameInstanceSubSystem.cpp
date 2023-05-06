#include "RTSGameInstanceSubSystem.h"

bool URTSGameInstanceSubSystem::GetPlayerColor(const uint8 Index, FLinearColor& Color)
{
	if (!PlayerColors.IsValidIndex(Index))
	{
		return false;
	}

	Color = PlayerColors[Index];

	return true;
}

bool URTSGameInstanceSubSystem::GetPlayerRace(const uint8 Index, URTSRace*& Race)
{
	if (!PlayerRaces.IsValidIndex(Index))
	{
		return false;
	}

	Race = PlayerRaces[Index];

	return true;
}
