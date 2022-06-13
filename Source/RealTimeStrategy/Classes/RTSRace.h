// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaceUnitData.h"
#include "Engine/DataAsset.h"
#include "RTSRace.generated.h"

/**
 * 
 */
UCLASS()
class REALTIMESTRATEGY_API URTSRace : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The name of the race */
	UPROPERTY(EditDefaultsOnly, Category="RTS")
	FText Name;

	UPROPERTY(EditDefaultsOnly, Category = "RTS", meta=(ShowOnlyInnerProperties))
	FRaceUnitData DefaultRaceUnitData;
};
