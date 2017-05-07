// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "TemplateGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UTemplateGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UTemplateGameInstance();

	// save across the game
	FString playerName;
	
	void SaveName(FString newName);

	// save across the game
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Template - Game Instance")
	int teamToSpawnIn;
	
	void SaveTeamNumber(int teamNumber);

};
