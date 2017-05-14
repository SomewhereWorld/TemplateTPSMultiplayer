// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "TemplateData.h"
#include "TemplateLobbyController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ATemplateLobbyController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ATemplateLobbyController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Template - Lobby Controller")
	FString GetName();

	UFUNCTION(BlueprintCallable, Category = "Template - Lobby Controller")
	int GetIndex();

	// Client on Server
	void AddAControllerToList(ATemplateLobbyController* newController);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAddAControllerToList(ATemplateLobbyController* newController);
	virtual void ServerAddAControllerToList_Implementation(ATemplateLobbyController* newController);
	virtual bool ServerAddAControllerToList_Validate(ATemplateLobbyController* newController);

	UFUNCTION(Reliable, Client, WithValidation)
	void ClientRefreshHUD(FLobbyControllerInfos newController);
	virtual void ClientRefreshHUD_Implementation(FLobbyControllerInfos newController);
	virtual bool ClientRefreshHUD_Validate(FLobbyControllerInfos newController);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Template - Lobby Controller")
	void RefreshHUD(FLobbyControllerInfos newController);

	UFUNCTION(Reliable, Client, WithValidation)
	void ClientAddPendingController(FLobbyControllerInfos newController);
	virtual void ClientAddPendingController_Implementation(FLobbyControllerInfos newController);
	virtual bool ClientAddPendingController_Validate(FLobbyControllerInfos newController);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Template - Lobby Controller")
	void CreateLobbyWidget();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Template - Lobby Controller")
	void AddPendingController(FLobbyControllerInfos newController);

	UFUNCTION(Reliable, Client, WithValidation)
	void ClientRetreiveController(FLobbyControllerInfos newController);
	virtual void ClientRetreiveController_Implementation(FLobbyControllerInfos newController);
	virtual bool ClientRetreiveController_Validate(FLobbyControllerInfos newController);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Template - Lobby Controller")
	void RetreiveController(FLobbyControllerInfos newController);

	UFUNCTION(Reliable, Client, WithValidation)
	void ClientUpdateController(FLobbyControllerInfos newController);
	virtual void ClientUpdateController_Implementation(FLobbyControllerInfos newController);
	virtual bool ClientUpdateController_Validate(FLobbyControllerInfos newController);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Template - Lobby Controller")
	void UpdateController(FLobbyControllerInfos newController);

	void SetControllerName(FString newName);

	UFUNCTION(Reliable, Client, WithValidation)
	void ClientSaveTeam(int teamToSave, const FString& theName);
	virtual void ClientSaveTeam_Implementation(int teamToSave, const FString& theName);
	virtual bool ClientSaveTeam_Validate(int teamToSave, const FString& theName);

private:

	UPROPERTY(replicated)
	FString _name;

	UPROPERTY(replicated)
	int _indexPos;
};
