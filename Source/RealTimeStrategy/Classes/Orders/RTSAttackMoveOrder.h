// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSOrder.h"
#include "RTSAttackMoveOrder.generated.h"

/**
 * 
 */
UCLASS()
class REALTIMESTRATEGY_API URTSAttackMoveOrder : public URTSOrder
{
	GENERATED_BODY()

public:
	URTSAttackMoveOrder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
