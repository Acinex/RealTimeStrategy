#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "Templates/SubclassOf.h"

#include "RTSGameMode.generated.h"


class URTSOwnerComponent;
class AController;

class ARTSPlayerAIController;
class ARTSPlayerController;
class ARTSPlayerStart;
class ARTSTeamInfo;


/**
* Game mode with RTS features, such as spawning initial units for each player.
*/
UCLASS(Category="RTS")
class REALTIMESTRATEGY_API ARTSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARTSGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	ARTSPlayerStart* FindRTSPlayerStart(AController* Player);

	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;

	virtual ARTSPlayerAIController* StartAIPlayer();

	UFUNCTION(BlueprintPure)
	FLinearColor GetPlayerColor(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void SetPlayerColors(TArray<FLinearColor> Colors);

	/** Spawns an actor of the specified type and transfers ownership to the specified player. */
	virtual AActor* SpawnActorForPlayer(TSubclassOf<AActor> ActorClass, AController* ActorOwner, const FTransform& SpawnTransform);

	/** Sets the specified player as the owner of the passed actor. */
	UFUNCTION(BlueprintCallable)
	void TransferOwnership(AActor* Actor, AController* NewOwner);

	/** Gets the teams of the current match. */
	TArray<ARTSTeamInfo*> GetTeams() const;

	/** Event when an actor has been killed. */
	virtual void NotifyOnActorKilled(AActor* Actor, AController* ActorOwner);

	/** Event when a player has been defeated. */
	virtual void NotifyOnPlayerDefeated(AController* Player);

	/** Event when a player has been defeated. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS", meta = (DisplayName = "OnPlayerDefeated"))
	void ReceiveOnPlayerDefeated(AController* Player);

protected:
	UPROPERTY(EditAnywhere, Category="RTS|Players")
	TArray<FLinearColor> PlayerColors;

private:
	/** Class of TeamInfo to spawn. */
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TSubclassOf<ARTSTeamInfo> TeamClass;

	/** Number of teams to create. */
	UPROPERTY(EditDefaultsOnly, Category = "Team", meta = (ClampMin = 0))
	uint8 NumTeams;

	/** AIController class to spawn for AI players. */
	UPROPERTY(EditDefaultsOnly, Category = AI)
	TSubclassOf<ARTSPlayerAIController> PlayerAIControllerClass;

	/** Number of AI players to spawn. */
	UPROPERTY(EditDefaultsOnly, Category = AI)
	int32 NumAIPlayers;

	/** Teams of the current match. */
	UPROPERTY()
	TArray<ARTSTeamInfo*> Teams;

	/** Gets the first player index that isn't assigned to any player. */
	uint8 GetAvailablePlayerIndex();

	UFUNCTION()
	void OnActorRegistered(UActorComponent* Component);

	/** Will transfer the ownership from every actor which has no owner currently */
	void CorrectAllDefaultOwnerStates();
	bool CorrectDefaultOwnerState(URTSOwnerComponent* OwnerComponent);
};
