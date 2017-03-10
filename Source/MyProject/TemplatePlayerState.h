// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "TemplatePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ATemplatePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	ATemplatePlayerState();

	void AddKill(int Amount);
	void AddDeath(int Amount);
	void AddScore(int Amount);
	void AddAssist(int Amount);

	UFUNCTION(BlueprintCallable, Category = "Template")
	int GetKills();
	UFUNCTION(BlueprintCallable, Category = "Template")
	int GetAssists();
	UFUNCTION(BlueprintCallable, Category = "Template")
	int GetDeaths();
	UFUNCTION(BlueprintCallable, Category = "Template")
	int GetScore();
	UFUNCTION(BlueprintCallable, Category = "Template")
	FString GetName();
	UFUNCTION(BlueprintCallable, Category = "Template")
	int GetPlayerTeamNumber();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps)const override;

protected:

	UPROPERTY(Replicated)
	int _playerScore;

	UPROPERTY(Replicated)
	FString _playerName;

	UPROPERTY(Replicated)
	int _death;

	UPROPERTY(Replicated)
	int _kills;
	
	UPROPERTY(Replicated)
	int _assist;

	UPROPERTY(Replicated)
	int _playerTeamNumber;
};
