#include "UI/RTSHUD.h"

#include "EngineUtils.h"

#include "RTSPlayerController.h"
#include "Combat/RTSHealthBarWidgetComponent.h"
#include "Construction/RTSConstructionSiteComponent.h"
#include "Construction/RTSConstructionProgressBarWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/RTSCollisionLibrary.h"
#include "Libraries/RTSGameplayLibrary.h"
#include "Production/RTSProductionComponent.h"
#include "Production/RTSProductionProgressBarWidgetComponent.h"
#include "UI/RTSFloatingCombatTextComponent.h"
#include "UI/RTSFloatingCombatTextData.h"
#include "UI/RTSHoveredActorWidgetComponent.h"


void ARTSHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawFloatingCombatTexts();
	DrawSelectionFrame();
	DrawHealthBars();
	DrawConstructionProgressBars();
	DrawProductionProgressBars();
	DrawHoveredActorWidget();
}

void ARTSHUD::BeginPlay()
{
	Super::BeginPlay();

	ComponentRegistry = UGameplayStatics::GetGameInstance(this)->GetSubsystem<URTSComponentRegistry>();
}

void ARTSHUD::NotifyDrawFloatingCombatText(AActor* Actor, const FString& Text, const FLinearColor& Color, float Scale, float Lifetime, float RemainingLifetime, float LifetimePercentage, float SuggestedTextLeft, float SuggestedTextTop)
{
	ReceiveDrawFloatingCombatText(
		Actor,
		Text,
		Color,
		Scale,
		Lifetime,
		RemainingLifetime,
		LifetimePercentage,
		SuggestedTextLeft,
		SuggestedTextTop);
}

void ARTSHUD::NotifyDrawSelectionFrame(float ScreenX, float ScreenY, float Width, float Height)
{
	ReceiveDrawSelectionFrame(ScreenX, ScreenY, Width, Height);
}

void ARTSHUD::NotifyHideSelectionFrame()
{
	ReceiveHideSelectionFrame();
}

FVector2D ARTSHUD::GetActorCenterOnScreen(AActor* Actor) const
{
	const FVector ProjectedLocation = Project(Actor->GetActorLocation());
	return FVector2D(ProjectedLocation.X, ProjectedLocation.Y);
}

FVector2D ARTSHUD::GetActorSizeOnScreen(AActor* Actor) const
{
	// Get actor position projected on HUD.
	const float ActorHeight = URTSCollisionLibrary::GetActorCollisionHeight(Actor);
	const float ActorWidth = URTSCollisionLibrary::GetActorCollisionSize(Actor);

	const FVector ActorTopPosition = Project(Actor->GetActorLocation() + (Actor->GetActorForwardVector() * ActorHeight));
	const FVector ActorBottomPosition = Project(Actor->GetActorLocation() - (Actor->GetActorForwardVector() * ActorHeight));
	const FVector ActorLeftPosition = Project(Actor->GetActorLocation() - (Actor->GetActorRightVector() * ActorWidth));
	const FVector ActorRightPosition = Project(Actor->GetActorLocation() + (Actor->GetActorRightVector() * ActorWidth));

	const float Width = FVector(ActorRightPosition - ActorLeftPosition).Size();
	const float Height = FVector(ActorTopPosition - ActorBottomPosition).Size();

	return FVector2D(Width, Height);
}

void ARTSHUD::DrawSelectionFrame()
{
	// Get selection frame.
	const ARTSPlayerController* PlayerController = Cast<ARTSPlayerController>(PlayerOwner);

	if (!PlayerController)
	{
		return;
	}

	FIntRect SelectionFrame;

	if (!PlayerController->GetSelectionFrame(SelectionFrame))
	{
		if (bWasDrawingSelectionFrame)
		{
			NotifyHideSelectionFrame();
		}

		bWasDrawingSelectionFrame = false;
		return;
	}

	// Draw selection frame.
	NotifyDrawSelectionFrame(
		SelectionFrame.Min.X,
		SelectionFrame.Min.Y,
		SelectionFrame.Width(),
		SelectionFrame.Height());

	bWasDrawingSelectionFrame = true;
}

void ARTSHUD::DrawFloatingCombatTexts()
{
	if (!bShowFloatingCombatTexts)
	{
		return;
	}

	TSet<TWeakObjectPtr<URTSFloatingCombatTextComponent>> FloatingCombatTextComponents = ComponentRegistry->GetComponents<URTSFloatingCombatTextComponent>();

	for (TWeakObjectPtr<URTSFloatingCombatTextComponent> FloatingCombatTextComponent : FloatingCombatTextComponents)
	{
		if (!FloatingCombatTextComponent.IsValid())
		{
			continue;
		}
		
		for (FRTSFloatingCombatTextData& TextData : FloatingCombatTextComponent->GetTexts())
		{
			// Calculate lifetime.
			const float ElapsedLifetime = TextData.Lifetime - TextData.RemainingLifetime;
			const float LifetimePercentage = ElapsedLifetime / TextData.Lifetime;

			// Calculate position.
			const FVector2D Center = GetActorCenterOnScreen(FloatingCombatTextComponent->GetOwner());
			const FVector2D Size = GetActorSizeOnScreen(FloatingCombatTextComponent->GetOwner());

			// Calculate color.
			FLinearColor TextColor = TextData.Color;

			if (bFadeOutFloatingCombatTexts)
			{
				TextColor.A = 1 - LifetimePercentage;
			}

			// Draw text.
			NotifyDrawFloatingCombatText(
				FloatingCombatTextComponent->GetOwner(),
				TextData.Text,
				TextColor,
				TextData.Scale,
				TextData.Lifetime,
				TextData.RemainingLifetime,
				LifetimePercentage,
				Center.X,
				Center.Y - (Size.Y / 2) - (FloatingCombatTextSpeed * ElapsedLifetime));
		}
	}
}

void ARTSHUD::DrawHealthBars() const
{
	const ARTSPlayerController* PlayerController = Cast<ARTSPlayerController>(PlayerOwner);

	if (!PlayerController)
	{
		return;
	}

	TSet<TWeakObjectPtr<URTSHealthBarWidgetComponent>> HealthBarWidgetComponents = ComponentRegistry->GetComponents<URTSHealthBarWidgetComponent>();

	for (TWeakObjectPtr<URTSHealthBarWidgetComponent> HealthBarWidgetComponent : HealthBarWidgetComponents)
	{
		if(!HealthBarWidgetComponent.IsValid())
		{
			continue;
		}

		AActor* Actor = HealthBarWidgetComponent->GetOwner();
		
		if (!URTSGameplayLibrary::IsFullyVisibleForLocalClient(Actor))
		{
			HealthBarWidgetComponent->SetVisibility(false);
			continue;
		}

		// Check override conditions.
		if (bAlwaysShowHealthBars || (bShowHotkeyHealthBars && PlayerController->IsHealthBarHotkeyPressed()))
		{
			// Draw all health bars.
			DrawHealthBar(HealthBarWidgetComponent.Get());
		}
		else if (bShowHoverHealthBars && Actor == PlayerController->GetHoveredActor())
		{
			// Draw health bar for hovered actor.
			DrawHealthBar(HealthBarWidgetComponent.Get());
		}
		else if (bShowSelectionHealthBars && PlayerController->GetSelectedActors().Contains(Actor))
		{
			// Draw health bars for selected actors.
			DrawHealthBar(HealthBarWidgetComponent.Get());
		}
		else
		{
			HealthBarWidgetComponent->SetVisibility(false);
		}
	}
}

void ARTSHUD::DrawHealthBar(URTSHealthBarWidgetComponent* HealthBarWidgetComponent) const
{
	if (!IsValid(HealthBarWidgetComponent))
	{
		return;
	}

	const FVector2D Size = GetActorSizeOnScreen(HealthBarWidgetComponent->GetOwner());

	HealthBarWidgetComponent->UpdatePositionAndSize(Size);
	HealthBarWidgetComponent->SetVisibility(true);
}

void ARTSHUD::DrawConstructionProgressBars() const
{
	const ARTSPlayerController* PlayerController = Cast<ARTSPlayerController>(PlayerOwner);

	if (!PlayerController)
	{
		return;
	}
	
	TSet<TWeakObjectPtr<URTSConstructionProgressBarWidgetComponent>> ConstructionProgressBarWidgetComponents = ComponentRegistry->GetComponents<URTSConstructionProgressBarWidgetComponent>();

	for (TWeakObjectPtr<URTSConstructionProgressBarWidgetComponent> ConstructionProgressBarWidgetComponent : ConstructionProgressBarWidgetComponents)
	{
		if(!ConstructionProgressBarWidgetComponent.IsValid())
		{
			continue;
		}

		AActor* Actor = ConstructionProgressBarWidgetComponent->GetOwner();

		if (!URTSGameplayLibrary::IsFullyVisibleForLocalClient(Actor))
		{
			ConstructionProgressBarWidgetComponent->SetVisibility(false);
			continue;
		}

		// Check override conditions.
		if (bAlwaysShowConstructionProgressBars || (bShowHotkeyConstructionProgressBars && PlayerController->IsConstructionProgressBarHotkeyPressed()))
		{
			// Draw all progress bars.
			DrawConstructionProgressBar(ConstructionProgressBarWidgetComponent.Get());
		}
		else if (bShowHoverConstructionProgressBars && Actor == PlayerController->GetHoveredActor())
		{
			// Draw progress bar for hovered actor.
			DrawConstructionProgressBar(ConstructionProgressBarWidgetComponent.Get());
		}
		else if (bShowSelectionConstructionProgressBars && PlayerController->GetSelectedActors().Contains(Actor))
		{
			// Draw progress bars for selected actors.
			DrawConstructionProgressBar(ConstructionProgressBarWidgetComponent.Get());
		}
		else
		{
			ConstructionProgressBarWidgetComponent->SetVisibility(false);
		}
	}
}

void ARTSHUD::DrawConstructionProgressBar(URTSConstructionProgressBarWidgetComponent* ConstructionProgressBarWidgetComponent) const
{
	// Check progress.
	const URTSConstructionSiteComponent* ConstructionSiteComponent = ConstructionProgressBarWidgetComponent->GetOwner()->FindComponentByClass<URTSConstructionSiteComponent>();

	if (!ConstructionSiteComponent)
	{
		return;
	}

	if (!ConstructionSiteComponent->IsConstructing())
	{
		return;
	}

	const FVector2D Size = GetActorSizeOnScreen(ConstructionProgressBarWidgetComponent->GetOwner());

	ConstructionProgressBarWidgetComponent->UpdatePositionAndSize(Size);
	ConstructionProgressBarWidgetComponent->SetVisibility(true);
}

void ARTSHUD::DrawHoveredActorWidget()
{
	const ARTSPlayerController* PlayerController = Cast<ARTSPlayerController>(PlayerOwner);

	if (!PlayerController)
	{
		return;
	}

	AActor* NewHoveredActor = PlayerController->GetHoveredActor();

	if (NewHoveredActor == OldHoveredActor)
	{
		return;
	}

	if (IsValid(OldHoveredActor))
	{
		URTSHoveredActorWidgetComponent* OldHoveredActorWidgetComponent =
			OldHoveredActor->FindComponentByClass<URTSHoveredActorWidgetComponent>();

		if (IsValid(OldHoveredActorWidgetComponent))
		{
			OldHoveredActorWidgetComponent->SetVisibility(false);
		}
	}

	if (IsValid(NewHoveredActor))
	{
		URTSHoveredActorWidgetComponent* NewHoveredActorWidgetComponent =
			NewHoveredActor->FindComponentByClass<URTSHoveredActorWidgetComponent>();

		if (IsValid(NewHoveredActorWidgetComponent))
		{
			const FVector2D Size = GetActorSizeOnScreen(NewHoveredActor);

			NewHoveredActorWidgetComponent->UpdatePositionAndSize(Size);
			NewHoveredActorWidgetComponent->UpdateData(NewHoveredActor);
			NewHoveredActorWidgetComponent->SetVisibility(true);
		}
	}

	OldHoveredActor = NewHoveredActor;
}

void ARTSHUD::DrawProductionProgressBars() const
{
	const ARTSPlayerController* PlayerController = Cast<ARTSPlayerController>(PlayerOwner);

	if (!PlayerController)
	{
		return;
	}
	
	TSet<TWeakObjectPtr<URTSProductionProgressBarWidgetComponent>> ProductionProgressBarWidgetComponents = ComponentRegistry->GetComponents<URTSProductionProgressBarWidgetComponent>();
	
	for (TWeakObjectPtr<URTSProductionProgressBarWidgetComponent> ProductionProgressBarWidgetComponent : ProductionProgressBarWidgetComponents)
	{
		if(!ProductionProgressBarWidgetComponent.IsValid())
		{
			continue;
		}

		AActor* Actor = ProductionProgressBarWidgetComponent->GetOwner();

		if (!URTSGameplayLibrary::IsFullyVisibleForLocalClient(Actor))
		{
			ProductionProgressBarWidgetComponent->SetVisibility(false);
			continue;
		}

		// Check override conditions.
		if (bAlwaysShowProductionProgressBars || (bShowHotkeyProductionProgressBars && PlayerController->IsProductionProgressBarHotkeyPressed()))
		{
			// Draw all progress bars.
			DrawProductionProgressBar(ProductionProgressBarWidgetComponent.Get());
		}
		else if (bShowHoverProductionProgressBars && Actor == PlayerController->GetHoveredActor())
		{
			// Draw progress bar for hovered actor.
			DrawProductionProgressBar(ProductionProgressBarWidgetComponent.Get());
		}
		else if (bShowSelectionProductionProgressBars && PlayerController->GetSelectedActors().Contains(Actor))
		{
			// Draw progress bars for selected actors.
			DrawProductionProgressBar(ProductionProgressBarWidgetComponent.Get());
		}
		else
		{
			ProductionProgressBarWidgetComponent->SetVisibility(false);
		}
	}
}

void ARTSHUD::DrawProductionProgressBar(URTSProductionProgressBarWidgetComponent* ProductionProgressBarWidgetComponent) const
{
	// Check progress.
	const URTSProductionComponent* ProductionComponent = ProductionProgressBarWidgetComponent->GetOwner()->FindComponentByClass<URTSProductionComponent>();

	if (!ProductionComponent)
	{
		return;
	}

	if (!ProductionComponent->IsProducing())
	{
		return;
	}

	// Draw progress bar.
	const FVector2D Size = GetActorSizeOnScreen(ProductionProgressBarWidgetComponent->GetOwner());

	ProductionProgressBarWidgetComponent->UpdatePositionAndSize(Size);
	ProductionProgressBarWidgetComponent->SetVisibility(true);
}
