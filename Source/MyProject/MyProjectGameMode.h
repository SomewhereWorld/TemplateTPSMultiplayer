// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "MyProjectGameMode.generated.h"

class ATemplateRespawn;

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyProjectGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRespawnPlayer(ATemplateCharacter* character);
	virtual void ServerRespawnPlayer_Implementation(ATemplateCharacter* character);
	virtual bool ServerRespawnPlayer_Validate(ATemplateCharacter* character);
	
	TArray<ATemplateRespawn*> _teamOneSpawn;
	TArray<ATemplateRespawn*> _teamTwoSpawn;

	TArray<ATemplateCharacter*> _teamOnePlayers;
	TArray<ATemplateCharacter*> _teamTwoPlayers;

	TArray<ATemplateCharacter*> _teamOneAlivePlayers;
	TArray<ATemplateCharacter*> _teamTwoAlivePlayers;

	void AddPlayerReady();

	void Respawn();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - GameMode")
	int nbPlayerNeeded;

	void PlayerDie(ATemplateCharacter* thePlayer);

private:
	void GetAllRespawn();

	void AffectTeams();

	void StartGame();
	
	void LaunchGame();

	int _indexRespawnOne;
	int _indexRespawnTwo;

	FTimerHandle _startTimerHandle;

	int _nbPlayersReady;
};
