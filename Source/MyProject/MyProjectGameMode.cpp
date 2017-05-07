// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MyProjectGameMode.h"
#include "TemplateRespawn.h"
#include "TemplateCharacter.h"
#include "TemplatePlayerState.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	nbPlayerNeeded = 6;
}

void AMyProjectGameMode::BeginPlay()
{
	_indexRespawnOne = 0;
	_indexRespawnTwo = 0;
	GetAllRespawn();

	_nbPlayersReady = 0;

	_teamOneWin = 0;
	_teamTwoWin = 0;

	_scoreToReach = 5;

	if(HasAuthority())
		GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::StartGame, 5.0f, false);
}

void AMyProjectGameMode::StartGame()
{
	AffectTeams();
}

void AMyProjectGameMode::ServerRespawnPlayer_Implementation(ATemplateCharacter* character)
{
	if (character->GetCastedPlayerState()->GetPlayerTeamNumber() == 0)
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

// TEAM 1 : _teamOnePlayers (Red)
// TEAM 0 : _teamTwoPlayers (Blue)

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

// executed on the server
void AMyProjectGameMode::PlayerDie(ATemplateCharacter* thePlayer)
{
	if (thePlayer->GetCastedPlayerState()->GetPlayerTeamNumber() == 1)
	{

		_teamOneAlivePlayers.Remove(thePlayer);

		if (_teamOneAlivePlayers.Num() <= 0)
		{
			_teamTwoWin++;
			for (auto i = 0; i < _teamOnePlayers.Num(); i++)
			{
				_teamOnePlayers[i]->ShowWinningTeam(0, _teamOneWin, _teamTwoWin);
			}
			for (auto i = 0; i < _teamTwoPlayers.Num(); i++)
			{
				_teamTwoPlayers[i]->ShowWinningTeam(0, _teamOneWin, _teamTwoWin);
			}
			if (_teamTwoWin < _scoreToReach)
			{
				GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::Respawn, 5.0f, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Team Two Win the Game !"));
				UE_LOG(LogTemp, Warning, TEXT("back to lobby..."));
			}
		}
		else
		{
			GivePower(thePlayer, 1);
		}
	}
	else
	{

		_teamTwoAlivePlayers.Remove(thePlayer);

		if (_teamTwoAlivePlayers.Num() <= 0)
		{
			_teamOneWin++;
			for (auto i = 0; i < _teamOnePlayers.Num(); i++)
			{
				_teamOnePlayers[i]->ShowWinningTeam(1, _teamOneWin, _teamTwoWin);
			}
			for (auto i = 0; i < _teamTwoPlayers.Num(); i++)
			{
				_teamTwoPlayers[i]->ShowWinningTeam(1, _teamOneWin, _teamTwoWin);
			}
			
			if (_teamOneWin < _scoreToReach)
			{
				GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::Respawn, 5.0f, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Team One Win the Game !"));
				UE_LOG(LogTemp, Warning, TEXT("back to lobby..."));
			}
		}
		else
		{
			GivePower(thePlayer, 0);
		}
	}
}

void AMyProjectGameMode::GivePower(ATemplateCharacter* thePlayer, int teamNumber)
{
	if (teamNumber == 1)
	{
		for (auto i = 0; i < _teamOneAlivePlayers.Num(); i++)
		{
			if (_teamOneAlivePlayers.Num() == 2)
				_teamOneAlivePlayers[i]->ChangePlayerPower(2, thePlayer->GetCastedPlayerState()->GetPower1());
			else if (_teamOneAlivePlayers.Num() == 1)
				_teamOneAlivePlayers[i]->ChangePlayerPower(3, thePlayer->GetCastedPlayerState()->GetPower1());

			_teamOneAlivePlayers[i]->DelayedRefreshPowerHUD();
		}
	}
	else
	{
		for (auto i = 0; i < _teamTwoAlivePlayers.Num(); i++)
		{
			if (_teamTwoAlivePlayers.Num() == 2)
			{
				_teamTwoAlivePlayers[i]->ChangePlayerPower(2, thePlayer->GetCastedPlayerState()->GetPower1());
			}
			else if (_teamTwoAlivePlayers.Num() == 1)
			{
				_teamTwoAlivePlayers[i]->ChangePlayerPower(3, thePlayer->GetCastedPlayerState()->GetPower1());
			}

			_teamTwoAlivePlayers[i]->DelayedRefreshPowerHUD();
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
			_teamOnePlayers[i]->ClientShowVignette(false);
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
		if (_teamTwoPlayers[i] && _teamTwoSpawn[i])
		{
			_teamTwoPlayers[i]->CleanWidgetWinningTeam();
			_teamTwoPlayers[i]->ClientShowVignette(false);
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
	UE_LOG(LogTemp, Warning, TEXT("needed %i players"), nbPlayerNeeded);
	if (_nbPlayersReady >= nbPlayerNeeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALL PLAYERS READY"));
		GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &AMyProjectGameMode::LaunchGame, 5.0f, false);
	}
}

void AMyProjectGameMode::LaunchGame()
{
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
	Respawn();
}