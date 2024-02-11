// Fill out your copyright notice in the Description page of Project Settings.


#include "Orders/RTSAttackMoveOrder.h"

#include "Libraries/RTSGameplayTagLibrary.h"


URTSAttackMoveOrder::URTSAttackMoveOrder(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	TargetType = ERTSOrderTargetType::ORDERTARGET_Location;
	GroupExecutionType = ERTSOrderGroupExecutionType::ORDERGROUPEXECUTION_All;

	IssueTagRequirements.SourceBlockedTags.AddTag(URTSGameplayTagLibrary::Status_Changing_Immobilized());
	IssueTagRequirements.SourceBlockedTags.AddTag(URTSGameplayTagLibrary::Status_Changing_Constructing());
}
