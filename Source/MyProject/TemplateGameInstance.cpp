// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateGameInstance.h"

UTemplateGameInstance::UTemplateGameInstance()
{
	playerName = "player1";
}

void UTemplateGameInstance::SaveName(FString newName)
{
	playerName = newName;
}