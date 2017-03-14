// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "TemplateData.h"
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

	UFUNCTION(BlueprintCallable, Category = "Template")
	void SetPlayerTeamNumber(int NewTeam);

	UFUNCTION(BlueprintCallable, Category = "Template")
	void SetPlayerPower(EPlayerPower NewPower);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps)const override;

	UFUNCTION(BlueprintCallable, Category = "Template")
	EPlayerPower GetPower1();
	UFUNCTION(BlueprintCallable, Category = "Template")
	EPlayerPower GetPower2();
	UFUNCTION(BlueprintCallable, Category = "Template")
	EPlayerPower GetPower3();

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

	UPROPERTY(Replicated)
	EPlayerPower _playerPower;

	UPROPERTY(Replicated)
	EPlayerPower _playerPower2;

	UPROPERTY(Replicated)
	EPlayerPower _playerPower3;
};
