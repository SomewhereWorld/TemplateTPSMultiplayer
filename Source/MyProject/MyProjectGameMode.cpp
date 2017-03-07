// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MyProjectGameMode.h"
#include "TemplateRespawn.h"
#include "TemplateCharacter.h"

void AMyProjectGameMode::BeginPlay()
{
	_indexRespawnOne = 0;
	_indexRespawnTwo = 0;
	GetAllRespawn();
}

void AMyProjectGameMode::ServerRespawn_Implementation(ATemplateCharacter* character)
{
	if (character->GetTeamNumber() == 0)
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
	}
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