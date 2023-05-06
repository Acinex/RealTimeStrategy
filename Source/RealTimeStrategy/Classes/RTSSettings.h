// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RTSSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "RTS"))
class REALTIMESTRATEGY_API URTSSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const URTSSettings* Get() { return CastChecked<URTSSettings>(StaticClass()->GetDefaultObject()); }

	/**
	 * Name to be used to pass the unit-color into the materials from the units owned by the players.
	 *
	 * If this is set to None, the units won't get a parameter at all
	 */
	UPROPERTY(Config, EditAnywhere, Category="Units")
	FName UnitColorMaterialParameterName;
	
	/**
	 * Name to be used to pass the unit-color into the materials for the particles spawned for each order the player issues.
	 *
	 * If this is set to None, the particles won't get a parameter at all
	 */
	UPROPERTY(Config, EditAnywhere, Category="Effects")
	FName OrderParticleMaterialParameterName;
};
