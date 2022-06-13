#include "RTSGameMode.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

#include "RTSGameState.h"
#include "RTSLog.h"
#include "RTSOwnerComponent.h"
#include "RTSPlayerAIController.h"
#include "RTSPlayerAdvantageComponent.h"
#include "RTSPlayerController.h"
#include "RTSPlayerStart.h"
#include "RTSPlayerState.h"
#include "RTSRace.h"
#include "RTSTeamInfo.h"
#include "Construction/RTSConstructionSiteComponent.h"
#include "Vision/RTSVisionInfo.h"


ARTSGameMode::ARTSGameMode(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	// Set reasonable default values.
	TeamClass = ARTSTeamInfo::StaticClass();
	NumTeams = 2;
}

void ARTSGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Parse options.
	FString NumAIPlayersString = UGameplayStatics::ParseOption(OptionsString, TEXT("NumAIPlayers"));

	if (!NumAIPlayersString.IsEmpty())
	{
		NumAIPlayers = FCString::Atoi(*NumAIPlayersString);
	}

	UE_LOG(LogRTS, Log, TEXT("NumAIPlayers = %i"), NumAIPlayers);

	// Spawn AI players.
	for (int32 Index = 0; Index < NumAIPlayers; ++Index)
	{
		ARTSPlayerAIController* NewAI = StartAIPlayer();

		if (NewAI != nullptr)
		{
			NewAI->PlayerState->SetPlayerName(FString::Printf(TEXT("AI Player %i"), Index + 1));
		}
	}

	URTSComponentRegistry* ComponentRegistry = GetGameInstance()->GetSubsystem<URTSComponentRegistry>();

	CorrectAllDefaultOwnerStates();

	ComponentRegistry->OnActorRegistered.AddDynamic(this, &ARTSGameMode::OnActorRegistered);
}

void ARTSGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameModeBase::InitGame(MapName, Options, ErrorMessage);

	// Set up teams.
	if (TeamClass == nullptr)
	{
		TeamClass = ARTSTeamInfo::StaticClass();
	}

	for (uint8 TeamIndex = 0; TeamIndex < NumTeams; ++TeamIndex)
	{
		// Add team.
		ARTSTeamInfo* NewTeam = GetWorld()->SpawnActor<ARTSTeamInfo>(TeamClass);
		NewTeam->SetTeamIndex(TeamIndex);

		if (Teams.Num() <= TeamIndex)
		{
			Teams.SetNum(TeamIndex + 1);
		}

		Teams[TeamIndex] = NewTeam;

		// Setup vision.
		ARTSVisionInfo* TeamVision = GetWorld()->SpawnActor<ARTSVisionInfo>();
		TeamVision->SetTeamIndex(TeamIndex);

		UE_LOG(LogRTS, Log, TEXT("Set up team %s (team index %i)."), *NewTeam->GetName(), TeamIndex);
	}
}

ARTSPlayerStart* ARTSGameMode::FindRTSPlayerStart(AController* Player)
{
	// Choose a player start.
	TArray<ARTSPlayerStart*> UnOccupiedStartPoints;
	TArray<ARTSPlayerStart*> OccupiedStartPoints;

	for (TActorIterator<ARTSPlayerStart> It(GetWorld()); It; ++It)
	{
		ARTSPlayerStart* PlayerStart = *It;

		if (PlayerStart->GetPlayer() == nullptr)
		{
			UnOccupiedStartPoints.Add(PlayerStart);
		}
		else
		{
			OccupiedStartPoints.Add(PlayerStart);
		}
	}

	if (UnOccupiedStartPoints.Num() > 0)
	{
		return UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
	}
	else if (OccupiedStartPoints.Num() > 0)
	{
		return OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
	}

	return nullptr;
}

void ARTSGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	ARTSPlayerStart* StartSpot = FindRTSPlayerStart(NewPlayer);
	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

void ARTSGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	// Spawn default pawns (most likely, this will be the camera pawn in our case).
	AGameModeBase::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

	// Verify parameters.
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	if (!StartSpot)
	{
		return;
	}

	ARTSGameState* RTSGameState = GetGameState<ARTSGameState>();

	if (!IsValid(RTSGameState))
	{
		return;
	}

	// Occupy start spot.
	ARTSPlayerStart* PlayerStart = Cast<ARTSPlayerStart>(StartSpot);

	if (PlayerStart)
	{
		UE_LOG(LogRTS, Log, TEXT("Start spot %s is now occupied by player %s."), *PlayerStart->GetName(), *NewPlayer->GetName());
		PlayerStart->SetPlayer(NewPlayer);
	}

	// Set team.
	if (PlayerStart->GetTeamIndex() >= Teams.Num())
	{
		UE_LOG(LogRTS, Warning, TEXT("Player start team index is %d, but game only has %d teams."), PlayerStart->GetTeamIndex(), Teams.Num());
	}
	else
	{
		Teams[PlayerStart->GetTeamIndex()]->AddToTeam(NewPlayer);
	}

	// Build spawn transform.
	// Don't allow initial actors to be spawned with any pitch or roll.
	FRotator SpawnRotation(ForceInit);
	const FRotator ActorRotation = StartSpot->GetActorRotation();
	SpawnRotation.Yaw = ActorRotation.Yaw;

	ARTSPlayerState* PlayerState = Cast<ARTSPlayerState>(NewPlayer->PlayerState);

	const uint8 PlayerIndex = GetAvailablePlayerIndex();
	PlayerState->SetPlayerIndex(PlayerIndex);
	PlayerState->SetColor(GetPlayerColor(PlayerIndex));

	if (URTSRace* Race = PlayerState->GetRace(); IsValid(Race))
	{
		const FRaceUnitData UnitData = GetRaceUnitData(Race);

		// Build spawn info.
		for (int32 Index = 0; Index < UnitData.InitialActors.Num(); ++Index)
		{
			const TSubclassOf<AActor> ActorClass = UnitData.InitialActors[Index];

			// Spawn actor.
			FVector SpawnLocation = StartSpot->GetActorLocation();

			if (Index < UnitData.InitialActorLocations.Num())
			{
				SpawnLocation += ActorRotation.RotateVector(UnitData.InitialActorLocations[Index]);
			}

			FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);

			// Finish construction of initial buildings immediately.
			if (const AActor* SpawnedActor = SpawnActorForPlayer(ActorClass, NewPlayer, SpawnTransform); SpawnedActor != nullptr)
			{
				if (URTSConstructionSiteComponent* ConstructionSiteComponent = SpawnedActor->FindComponentByClass<URTSConstructionSiteComponent>(); ConstructionSiteComponent != nullptr)
				{
					ConstructionSiteComponent->FinishConstruction();
				}
			}
		}
	}
}

ARTSPlayerAIController* ARTSGameMode::StartAIPlayer()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient; // We never want to save player controllers into a map
	SpawnInfo.bDeferConstruction = true;
	ARTSPlayerAIController* NewAI = GetWorld()->SpawnActor<ARTSPlayerAIController>(PlayerAIControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
	if (NewAI)
	{
		UGameplayStatics::FinishSpawningActor(NewAI, FTransform(FRotator::ZeroRotator, FVector::ZeroVector));

		UE_LOG(LogRTS, Log, TEXT("Spawned AI player %s."), *NewAI->GetName());
	}
	else
	{
		UE_LOG(LogRTS, Error, TEXT("Failed to spawn AI player."));
		return nullptr;
	}

	RestartPlayer(NewAI);
	return NewAI;
}

FLinearColor ARTSGameMode::GetPlayerColor(const uint8 Index)
{
	if (!PlayerColors.IsValidIndex(Index))
	{
		return FLinearColor::Black;
	}

	return PlayerColors[Index];
}

void ARTSGameMode::SetPlayerColors(const TArray<FLinearColor> Colors)
{
	PlayerColors = Colors;

	for (const ARTSTeamInfo* Team : Teams)
	{
		TArray<AController*> Controllers = Team->GetTeamMembers();
		for (const AController* Controller : Controllers)
		{
			ARTSPlayerState* PlayerState = Controller->GetPlayerState<ARTSPlayerState>();
			PlayerState->SetColor(GetPlayerColor(PlayerState->GetPlayerIndex()));
		}
	}
}

AActor* ARTSGameMode::SpawnActorForPlayer(TSubclassOf<AActor> ActorClass, AController* ActorOwner, const FTransform& SpawnTransform)
{
	// Spawn actor.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass->GetDefaultObject()->GetClass(), SpawnTransform, SpawnParams);

	// Set owning player.
	if (SpawnedActor && ActorOwner)
	{
		UE_LOG(LogRTS, Log, TEXT("Spawned %s for player %s at %s."), *SpawnedActor->GetName(), *ActorOwner->GetName(), *SpawnTransform.GetLocation().ToString());

		// Set owning player.
		TransferOwnership(SpawnedActor, ActorOwner);
	}

	return SpawnedActor;
}

void ARTSGameMode::TransferOwnership(AActor* Actor, AController* NewOwner)
{
	if (!Actor || !NewOwner)
	{
		return;
	}

	// Set owning player.
	Actor->SetOwner(NewOwner);

	URTSOwnerComponent* OwnerComponent = Actor->FindComponentByClass<URTSOwnerComponent>();

	if (OwnerComponent)
	{
		OwnerComponent->SetPlayerOwner(NewOwner);
	}

	UE_LOG(LogRTS, Log, TEXT("Player %s is now owning %s."), *NewOwner->GetName(), *Actor->GetName());

	// Check for god mode.
	const URTSPlayerAdvantageComponent* PlayerAdvantageComponent = NewOwner->FindComponentByClass<URTSPlayerAdvantageComponent>();

	if (PlayerAdvantageComponent)
	{
		APawn* Pawn = Cast<APawn>(Actor);

		if (Pawn)
		{
			Pawn->SetCanBeDamaged(!PlayerAdvantageComponent->IsGodModeEnabled());
		}
	}
}

TArray<ARTSTeamInfo*> ARTSGameMode::GetTeams() const
{
	return Teams;
}

void ARTSGameMode::NotifyOnActorKilled(AActor* Actor, AController* ActorOwner)
{
	if (const ARTSPlayerController* OwningPlayer = Cast<ARTSPlayerController>(ActorOwner); OwningPlayer == nullptr)
	{
		if (const ARTSPlayerAIController* OwningAIPlayer = Cast<ARTSPlayerAIController>(ActorOwner); OwningAIPlayer == nullptr)
		{
			return;
		}
	}

	const ARTSPlayerState* PlayerState = ActorOwner->GetPlayerState<ARTSPlayerState>();
	if (!IsValid(PlayerState))
	{
		return;
	}

	URTSRace* Race = PlayerState->GetRace();


	const FRaceUnitData Data = GetRaceUnitData(Race);

	if (!IsValid(Race) || Data.DefeatConditionActorClasses.Num() <= 0)
	{
		return;
	}

	// Check if any required actors are still alive.
	for (const AActor* OwnedActor : ActorOwner->Children)
	{
		if (Data.DefeatConditionActorClasses.Contains(OwnedActor->GetClass()))
		{
			return;
		}
	}

	UE_LOG(LogRTS, Log, TEXT("Player %s does not control any required actors anymore and has been defeated."), *ActorOwner->GetName());

	// Notify listeners.
	NotifyOnPlayerDefeated(ActorOwner);
}

void ARTSGameMode::NotifyOnPlayerDefeated(AController* Player)
{
	ReceiveOnPlayerDefeated(Player);
}

FRaceUnitData ARTSGameMode::GetRaceUnitData(URTSRace* Race)
{
	if (FRaceUnitData* UnitData = RaceUnitData.Find(Race))
	{
		return *UnitData;
	}

	return Race->DefaultRaceUnitData;
}

uint8 ARTSGameMode::GetAvailablePlayerIndex()
{
	const UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return ARTSPlayerState::PLAYER_INDEX_NONE;
	}

	uint8 PlayerIndex = 0;
	bool bPlayerIndexInUse = false;

	do
	{
		bPlayerIndexInUse = false;

		for (TActorIterator<ARTSPlayerState> PlayerIt(World); PlayerIt; ++PlayerIt)
		{
			const ARTSPlayerState* PlayerState = *PlayerIt;

			if (PlayerState->GetPlayerIndex() == PlayerIndex)
			{
				bPlayerIndexInUse = true;
				++PlayerIndex;
				break;
			}
		}
	}
	while (bPlayerIndexInUse && PlayerIndex < ARTSPlayerState::PLAYER_INDEX_NONE);

	return PlayerIndex;
}

void ARTSGameMode::OnActorRegistered(UActorComponent* Component)
{
	CorrectDefaultOwnerState(Cast<URTSOwnerComponent>(Component));
}

void ARTSGameMode::CorrectAllDefaultOwnerStates()
{
	URTSComponentRegistry* ComponentRegistry = GetGameInstance()->GetSubsystem<URTSComponentRegistry>();

	TSet<TWeakObjectPtr<URTSOwnerComponent>> OwnerComponents = ComponentRegistry->GetComponents<URTSOwnerComponent>();
	for (TSet<TWeakObjectPtr<URTSOwnerComponent>>::TIterator ComponentIterator = OwnerComponents.CreateIterator(); ComponentIterator; ++ComponentIterator)
	{
		if (CorrectDefaultOwnerState(ComponentIterator->Get()))
		{
			ComponentIterator.RemoveCurrent();
		}
	}
}

bool ARTSGameMode::CorrectDefaultOwnerState(URTSOwnerComponent* OwnerComponent)
{
	if (!IsValid(OwnerComponent) || IsValid(OwnerComponent->GetPlayerOwner()) || OwnerComponent->GetInitialOwnerPlayerIndex() == ARTSPlayerState::PLAYER_INDEX_NONE)
	{
		return false;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = Iterator->Get();
		const ARTSPlayerState* PlayerState = Controller->GetPlayerState<ARTSPlayerState>();

		if (!IsValid(PlayerState))
		{
			continue;
		}

		if (OwnerComponent->GetInitialOwnerPlayerIndex() == PlayerState->GetPlayerIndex())
		{
			TransferOwnership(OwnerComponent->GetOwner(), Controller);
			return true;
		}
	}

	return false;
}
