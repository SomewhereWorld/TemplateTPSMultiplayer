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
	_playerPower = EPlayerPower::None;
	_playerPower2 = EPlayerPower::None;
	_playerPower3 = EPlayerPower::None;
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
	DOREPLIFETIME(ATemplatePlayerState, _playerPower);
	DOREPLIFETIME(ATemplatePlayerState, _playerPower2);
	DOREPLIFETIME(ATemplatePlayerState, _playerPower3);
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

void ATemplatePlayerState::SetPlayerTeamNumber(int NewTeam)
{
	_playerTeamNumber = NewTeam;
}

void ATemplatePlayerState::SetPlayerPower(int phase, EPlayerPower NewPower)
{
	if(phase == 1)
		_playerPower = NewPower;
	else if (phase == 2)
		_playerPower2 = NewPower;
	else if (phase == 3)
		_playerPower3 = NewPower;
}

EPlayerPower ATemplatePlayerState::GetPower1()
{
	return _playerPower;
}

EPlayerPower ATemplatePlayerState::GetPower2()
{
	return _playerPower2;
}

EPlayerPower ATemplatePlayerState::GetPower3()
{
	return _playerPower3;
}