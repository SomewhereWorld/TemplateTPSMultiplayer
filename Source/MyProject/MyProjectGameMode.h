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
	void ServerRespawn(ATemplateCharacter* character);
	virtual void ServerRespawn_Implementation(ATemplateCharacter* character);
	virtual bool ServerRespawn_Validate(ATemplateCharacter* character);
	
	TArray<ATemplateRespawn*> _teamOneSpawn;
	TArray<ATemplateRespawn*> _teamTwoSpawn;

private:
	void GetAllRespawn();

	void AffectTeams();
	
	int _indexRespawnOne;
	int _indexRespawnTwo;

	FTimerHandle _startTimerHandle;
};
