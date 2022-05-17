#include "RTSSelectableComponent.h"

#include "Components/DecalComponent.h"
#include "GameFramework/Actor.h"
#include "Libraries/RTSCollisionLibrary.h"

URTSSelectableComponent::URTSSelectableComponent()
{
	// Rotate decal to face ground.
	SetRelativeRotation(FRotator::MakeFromEuler(FVector(0.0f, -90.0f, 0.0f)));
}

void URTSSelectableComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	if (bOverrideDecalSize)
	{
		// Calculate decal size.
		const float DecalHeight = URTSCollisionLibrary::GetActorCollisionHeight(Owner);
		const float DecalRadius = URTSCollisionLibrary::GetActorCollisionSize(Owner);

		DecalSize = FVector(DecalHeight, DecalRadius, DecalRadius);
	}

	SelectionCircleMaterialInstance = CreateDynamicMaterialInstance();
	SetHiddenInGame(true);
}

void URTSSelectableComponent::PostLoad()
{
	Super::PostLoad();
	
	if (!IsValid(SelectionCircleMaterial_DEPRECATED))
	{
		return;
	}
	
	DecalMaterial = SelectionCircleMaterial_DEPRECATED;
	SelectionCircleMaterial_DEPRECATED = nullptr;
}

void URTSSelectableComponent::SelectActor()
{
	if (bSelected)
	{
		return;
	}

	bSelected = true;

	// Update selection circle.
	SetHiddenInGame(false);

	// Notify listeners.
	OnSelected.Broadcast(GetOwner());
}

void URTSSelectableComponent::DeselectActor()
{
	if (!bSelected)
	{
		return;
	}

	bSelected = false;

	// Update selection circles.
	SetHiddenInGame(true);

	// Notify listeners.
	OnDeselected.Broadcast(GetOwner());
}

bool URTSSelectableComponent::IsSelected() const
{
	return bSelected;
}

void URTSSelectableComponent::HoverActor()
{
	if (bHovered)
	{
		return;
	}

	bHovered = true;

	// Notify listeners.
	OnHovered.Broadcast(GetOwner());
}

void URTSSelectableComponent::UnhoverActor()
{
	if (!bHovered)
	{
		return;
	}

	bHovered = false;

	// Notify listeners.
	OnUnhovered.Broadcast(GetOwner());
}

bool URTSSelectableComponent::IsHovered() const
{
	return bHovered;
}

int32 URTSSelectableComponent::GetSelectionPriority() const
{
	return SelectionPriority;
}

USoundCue* URTSSelectableComponent::GetSelectedSound() const
{
	return SelectedSound;
}
