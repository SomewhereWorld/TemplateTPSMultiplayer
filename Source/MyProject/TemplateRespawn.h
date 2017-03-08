// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TemplateRespawn.generated.h"

UCLASS()
class MYPROJECT_API ATemplateRespawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATemplateRespawn();

	int GetTeamNumber();

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Template")
	UCapsuleComponent* _capsuleComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Template")
	UArrowComponent* _directionComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template")
	int _teamNumber;
};
