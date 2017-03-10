// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplatePlayerState.h"

ATemplatePlayerState::ATemplatePlayerState()
{
	_playerScore = 0;
	_playerName = "UNKNOWN";
	_death = 0;
	_kills = 0;
	_assist = 0;
	_playerTeamNumber = 0;
}

void ATemplatePlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATemplatePlayerState, _playerName);
	DOREPLIFETIME(ATemplatePlayerState, _kills);
	DOREPLIFETIME(ATemplatePlayerState, _assist);
	DOREPLIFETIME(ATemplatePlayerState, _death);
	DOREPLIFETIME(ATemplatePlayerState, _playerScore);
	DOREPLIFETIME(ATemplatePlayerState, _playerTeamNumber);
}

void ATemplatePlayerState::AddKill(int Amount)
{
	_kills += Amount;
	AddScore(10);
}

void ATemplatePlayerState::AddDeath(int Amount)
{
	_death += Amount;
}

void ATemplatePlayerState::AddScore(int Amount)
{
	_playerScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("_kills %d, _score %d"), _kills, _playerScore);
}

void ATemplatePlayerState::AddAssist(int Amount)
{
	_assist += Amount;
	AddScore(5);
}

int ATemplatePlayerState::GetKills()
{
	return _kills;
}

int ATemplatePlayerState::GetAssists()
{
	return _assist;
}

int ATemplatePlayerState::GetDeaths()
{
	return _death;
}

int ATemplatePlayerState::GetScore()
{
	return _playerScore;
}

FString ATemplatePlayerState::GetName()
{
	return _playerName;
}

int ATemplatePlayerState::GetPlayerTeamNumber()
{
	return _playerTeamNumber;
}