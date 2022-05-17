#pragma once

#include "CoreMinimal.h"
#include "RTSComponentRegistry.h"
#include "Components/DecalComponent.h"

#include "RTSSelectableComponent.generated.h"


class UMaterialInterface;
class UMaterialInstanceDynamic;
class USoundCue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSSelectableComponentSelectedSignature, AActor*, Actor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSSelectableComponentDeselectedSignature, AActor*, Actor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSSelectableComponentHoveredSignature, AActor*, Actor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSSelectableComponentUnhoveredSignature, AActor*, Actor);


/**
 * Allows selecting the actor, e.g. by left-click.
 */
UCLASS(ClassGroup="RTS", Category="RTS", meta=(BlueprintSpawnableComponent))
class REALTIMESTRATEGY_API URTSSelectableComponent : public UDecalComponent
{
	GENERATED_BODY()

public:
	URTSSelectableComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostLoad() override;

	/** Selects the unit for the local player. */
	UFUNCTION(BlueprintCallable)
	void SelectActor();

	/** Deselects the unit for the local player. */
	UFUNCTION(BlueprintCallable)
	void DeselectActor();

	/** Checks whether the unit is currently selected by the local player, or not. */
	UFUNCTION(BlueprintPure)
	bool IsSelected() const;

	/** Hovers the unit for the local player. */
	void HoverActor();

	/** Unhovers the unit for the local player. */
	void UnhoverActor();

	/** Checks whether the unit is currently hovered by the local player, or not. */
	UFUNCTION(BlueprintPure)
	bool IsHovered() const;

	/** Gets the sort index for selected units. */
	UFUNCTION(BlueprintPure)
	int32 GetSelectionPriority() const;

	/** Gets the sound to play when the actor is selected. */
	USoundCue* GetSelectedSound() const;

	/** Event when the actor has been deselected. */
	UPROPERTY(BlueprintAssignable, Category = "RTS")
	FRTSSelectableComponentDeselectedSignature OnDeselected;

	/** Event when the actor has been selected. */
	UPROPERTY(BlueprintAssignable, Category = "RTS")
	FRTSSelectableComponentSelectedSignature OnSelected;

	/** Event when the actor has been hovered. */
	UPROPERTY(BlueprintAssignable, Category = "RTS")
	FRTSSelectableComponentHoveredSignature OnHovered;

	/** Event when the actor has been unhovered. */
	UPROPERTY(BlueprintAssignable, Category = "RTS")
	FRTSSelectableComponentUnhoveredSignature OnUnhovered;

protected:
	/** Defines if the Decal should be sized by the collision */
	UPROPERTY(EditDefaultsOnly, Category="RTS")
	bool bOverrideDecalSize = false;

private:
	/** Sort index for selected units. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	int32 SelectionPriority;

	/** Material for rendering the selection circle of the actor. */
	UPROPERTY()
	UMaterialInterface* SelectionCircleMaterial_DEPRECATED;

	/** Sound to play when the actor is selected. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	USoundCue* SelectedSound;

	/** Whether the unit is currently selected by the local player, or not. */
	bool bSelected;

	/** Whether the unit is currently hovered by the local player, or not. */
	bool bHovered;

	/** Material instance for rendering the selection circle of the actor. */
	UPROPERTY()
	UMaterialInstanceDynamic* SelectionCircleMaterialInstance;

	UPROPERTY()
	URTSComponentRegistry* ComponentRegistry;
};
