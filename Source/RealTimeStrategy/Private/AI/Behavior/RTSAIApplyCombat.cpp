// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Behavior/RTSAIApplyCombat.h"

URTSAIApplyCombat::URTSAIApplyCombat(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	NodeName = "AI Apply Combat";
}

EBTNodeResult::Type URTSAIApplyCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}
