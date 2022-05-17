#pragma once

#include "CoreMinimal.h"

#include "Components/WidgetComponent.h"

#include "RTSActorWidgetComponent.generated.h"

class URTSComponentRegistry;

/**
* Adds a customizable UI widget to the actor.
*/
UCLASS(Category="RTS")
class REALTIMESTRATEGY_API URTSActorWidgetComponent : public UWidgetComponent
{
    GENERATED_BODY()

public:
    URTSActorWidgetComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Event when the size of the actor on screen been calculated from its collision size. */
    UFUNCTION(BlueprintImplementableEvent, Category = "RTS")
    void UpdatePositionAndSize(const FVector2D& ActorScreenSize);

protected:
    UPROPERTY()
	URTSComponentRegistry* ComponentRegistry;
};
