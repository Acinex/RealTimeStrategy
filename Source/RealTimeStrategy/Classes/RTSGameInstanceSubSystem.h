#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RTSGameInstanceSubSystem.generated.h"

class URTSRace;
/**
 * 
 */
UCLASS()
class REALTIMESTRATEGY_API URTSGameInstanceSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	bool GetPlayerColor(const uint8 Index, FLinearColor& Color);

	UFUNCTION(BlueprintPure)
	bool GetPlayerRace(const uint8 Index, URTSRace*& Race);
	
private:
	UPROPERTY(EditAnywhere, Category="RTS|Players")
	TArray<FLinearColor> PlayerColors;

	UPROPERTY(EditDefaultsOnly, Category="RTS|Races")
	TArray<URTSRace*> PlayerRaces;
};
