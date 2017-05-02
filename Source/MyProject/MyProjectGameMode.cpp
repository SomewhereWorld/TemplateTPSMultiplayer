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
		GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::StartGame, 5.0f, false);
}

void AMyProjectGameMode::StartGame()
{
	AffectTeams();
	Respawn();
}

void AMyProjectGameMode::ServerRespawnPlayer_Implementation(ATemplateCharacter* character)
{
	if (character->GetTeamNumber() == 0)
	{
		int index = FMath::RandRange(0, _teamOneSpawn.Num() - 1);
		character->SetActorLocation(_teamOneSpawn[index]->GetActorLocation());
	}
	else
	{
		int index = FMath::RandRange(0, _teamTwoSpawn.Num() - 1);
		character->SetActorLocation(_teamTwoSpawn[index]->GetActorLocation());
	}
}

bool AMyProjectGameMode::ServerRespawnPlayer_Validate(ATemplateCharacter* character)
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

	for (auto i = 0; i < allPlayerStates.Num(); i++)
	{
		if (i % 2 == 1)
		{
			if (allPlayerStates[i] != nullptr)
			{
				allPlayerStates[i]->SetPlayerTeamNumber(1);
				
			}
			_teamOnePlayers.Add(allCharacters[i]);
		}
		else
		{
			_teamTwoPlayers.Add(allCharacters[i]);
		}
	}
}

void AMyProjectGameMode::PlayerDie(ATemplateCharacter* thePlayer)
{
	if (thePlayer->GetTeamNumber() == 0)
	{
		_teamOneAlivePlayers.Remove(thePlayer);
		if (_teamOneAlivePlayers.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("TEAM TWO WIN"));
			for (auto i = 0; i < _teamOnePlayers.Num(); i++)
			{
				_teamOnePlayers[i]->ShowWinningTeam(1);
			}
			for (auto i = 0; i < _teamTwoPlayers.Num(); i++)
			{
				_teamTwoPlayers[i]->ShowWinningTeam(1);
			}
			GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::Respawn, 5.0f, false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("_teamOneAlivePlayers %i"), _teamOneAlivePlayers.Num());
		}
	}
	else
	{
		_teamTwoAlivePlayers.Remove(thePlayer);
		if (_teamTwoAlivePlayers.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("TEAM ONE WIN"));
			for (auto i = 0; i < _teamOnePlayers.Num(); i++)
			{
				_teamOnePlayers[i]->ShowWinningTeam(0);
			}
			for (auto i = 0; i < _teamTwoPlayers.Num(); i++)
			{
				_teamTwoPlayers[i]->ShowWinningTeam(0);
			}
			GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::Respawn, 5.0f, false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("_teamTwoAlivePlayers %i"), _teamTwoAlivePlayers.Num());
		}
	}
}

void AMyProjectGameMode::Respawn()
{
	for (auto i = 0; i < _teamOnePlayers.Num(); i++)
	{
		if (_teamOnePlayers[i] && _teamOneSpawn[i])
		{
			_teamOnePlayers[i]->CleanWidgetWinningTeam();
			_teamOnePlayers[i]->ResetStats();
			_teamOnePlayers[i]->SetActorLocation(_teamOneSpawn[i]->GetActorLocation());
			_teamOneAlivePlayers.Add(_teamOnePlayers[i]);
		}
		else
		{
			if (!_teamOnePlayers[i])
			{
				UE_LOG(LogTemp, Warning, TEXT("_teamOnePlayers %i"), i);
			}
			if (!_teamOneSpawn[i])
			{
				UE_LOG(LogTemp, Warning, TEXT("_teamOneSpawn %i"), i);
			}
		}
	}

	for (auto i = 0; i < _teamTwoPlayers.Num(); i++)
	{
		if (_teamTwoPlayers[i] && _teamOneSpawn[i])
		{
			_teamTwoPlayers[i]->CleanWidgetWinningTeam();
			_teamTwoPlayers[i]->ResetStats();
			_teamTwoPlayers[i]->SetActorLocation(_teamTwoSpawn[i]->GetActorLocation());
			_teamTwoAlivePlayers.Add(_teamTwoPlayers[i]);
		}
		else
		{
			if (!_teamTwoPlayers[i])
			{
				UE_LOG(LogTemp, Warning, TEXT("_teamTwoPlayers %i"), i);
			}
			if (!_teamTwoSpawn[i])
			{
				UE_LOG(LogTemp, Warning, TEXT("_teamTwoSpawn %i"), i);
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