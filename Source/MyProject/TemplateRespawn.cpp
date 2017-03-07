// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateRespawn.h"


// Sets default values
ATemplateRespawn::ATemplateRespawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	_capsuleComponent->SetupAttachment(RootComponent);

	_directionComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("direction Component"));
	_directionComponent->SetupAttachment(_capsuleComponent);

	_teamNumber = 0;
}

int ATemplateRespawn::GetTeamNumber()
{
	return _teamNumber;
}