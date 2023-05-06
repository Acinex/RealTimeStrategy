#include "RTSPlayerStart.h"

int32 ARTSPlayerStart::GetTeamIndex() const
{
	return TeamIndex;
}

AController* ARTSPlayerStart::GetPlayer() const
{
	return Player;
}

uint8 ARTSPlayerStart::GetPlayerIndex() const
{
	return PlayerIndex;
}

void ARTSPlayerStart::SetPlayer(AController* InPlayer)
{
	Player = InPlayer;

	if (!bUseActorRotationForController)
	{
		if (IsValid(InPlayer->GetPawn()))
		{
			InPlayer->GetPawn()->SetActorRotation(ControllerRotation);
		}
		InPlayer->SetControlRotation(ControllerRotation);
	}
}

FRotator ARTSPlayerStart::GetControllerRotation() const
{
	if (bUseActorRotationForController)
	{
		return GetActorRotation();
	}

	return ControllerRotation;
}
