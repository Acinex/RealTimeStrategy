// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UObject/Object.h"
#include "RTSAIApplyCombat.generated.h"

/**
 * 
 */
UCLASS(DisplayName="AI Apply Combat", Category="RTS")
class REALTIMESTRATEGY_API URTSAIApplyCombat : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	explicit URTSAIApplyCombat(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
