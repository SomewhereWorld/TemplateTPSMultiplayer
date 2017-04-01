// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MyProjectGameMode.h"
#include "TemplateRespawn.h"
#include "TemplateCharacter.h"
#include "TemplatePlayerState.h"

void AMyProjectGameMode::BeginPlay()
{
	_indexRespawnOne = 0;
	_indexRespawnTwo = 0;
	GetAllRespawn();

	_nbPlayersReady = 0;
	nbPlayerNeeded = 6;

	if(HasAuthority())
		GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::AffectTeams, 5.0f, false);
}

void AMyProjectGameMode::ServerRespawn_Implementation(ATemplateCharacter* character)
{
	/*if (character->GetTeamNumber() == 0)
	{
		character->SetActorLocation(_teamOneSpawn[_indexRespawnOne]->GetActorLocation());
		_indexRespawnOne++;
		if (_indexRespawnOne >= _teamOneSpawn.Num())
		{
			_indexRespawnOne = 0;
		}
	}
	else if (character->GetTeamNumber() == 1)
	{
		character->SetActorLocation(_teamTwoSpawn[_indexRespawnTwo]->GetActorLocation());
		_indexRespawnTwo++;
		if (_indexRespawnTwo >= _teamTwoSpawn.Num())
		{
			_indexRespawnTwo = 0;
		}
	}*/
}

bool AMyProjectGameMode::ServerRespawn_Validate(ATemplateCharacter* character)
{
	return true;
}

void AMyProjectGameMode::GetAllRespawn()
{
	for (TActorIterator<ATemplateRespawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ATemplateRespawn* respawn = Cast<ATemplateRespawn>(*ActorItr);
		if (respawn)
		{
			if (respawn->GetTeamNumber() == 0)
			{
				_teamOneSpawn.Add(respawn);
			}
			else
			{
				_teamTwoSpawn.Add(respawn);
			}
		}
	}
}

void AMyProjectGameMode::AffectTeams()
{
	TArray<ATemplatePlayerState*> allPlayerStates;
	TArray<ATemplateCharacter*> allCharacters;
	for (TActorIterator<ATemplateCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ATemplateCharacter* aPlayer = Cast<ATemplateCharacter>(*ActorItr);
		if (aPlayer)
		{
			allCharacters.Add(aPlayer);
			allPlayerStates.Add(aPlayer->GetCastedPlayerState());
		}
	}

	int indexP0 = 0;
	int indexP1 = 0;

	for (auto i = 0; i < allPlayerStates.Num(); i++)
	{
		if (i % 2 == 1)
		{
			if (allPlayerStates[i] != nullptr)
			{
				allPlayerStates[i]->SetPlayerTeamNumber(1);
				
			}
			if (_teamOneSpawn[indexP0])
			{
				allCharacters[i]->SetActorLocation(_teamOneSpawn[indexP0]->GetActorLocation());
				indexP0++;
			}
		}
		else
		{
			if (_teamTwoSpawn[indexP1])
			{
				allCharacters[i]->SetActorLocation(_teamTwoSpawn[indexP1]->GetActorLocation());
				indexP1++;
			}
		}
	}
}

void AMyProjectGameMode::AddPlayerReady()
{
	_nbPlayersReady++;
	UE_LOG(LogTemp, Warning, TEXT("A PLAYER READY"));
	if (_nbPlayersReady >= nbPlayerNeeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALL PLAYERS READY"));
		// All players are ready to start
		for (TActorIterator<ATemplateCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ATemplateCharacter* aPlayer = Cast<ATemplateCharacter>(*ActorItr);
			if (aPlayer)
			{
				aPlayer->SetClientState(EClientState::Alive);
				aPlayer->LaunchGame();
			}
		}
	}
}