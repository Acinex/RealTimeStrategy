#pragma once

#include "CoreMinimal.h"

#include "RTSSelfRegisteringActorComponent.h"
#include "RTSGameplayTagsProvider.h"

#include "GameplayTagContainer.h"

#include "RTSActorComponent.generated.h"


/** Base class for actor components with additional functionality, e.g. initial gameplay tags. */
UCLASS(ClassGroup="RTS", Category="RTS", meta = (BlueprintSpawnableComponent))
class REALTIMESTRATEGY_API URTSActorComponent : public URTSSelfRegisteringActorComponent, public IRTSGameplayTagsProvider
{
	GENERATED_BODY()

public:
	virtual void AddGameplayTags(FGameplayTagContainer& InOutTagContainer) override;

protected:
	/** Gameplay tags to add to the actor on BeginPlay. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	FGameplayTagContainer InitialGameplayTags;
};
