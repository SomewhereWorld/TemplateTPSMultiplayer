// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateLobbyController.h"
#include "TemplateGameInstance.h"
#include "TemplateLobbyGameMode.h"
#include "TemplateData.h"

ATemplateLobbyController::ATemplateLobbyController()
{
	_name = "UNKNOWN";
	_indexPos = 0;

	bReplicates = true;
}

void ATemplateLobbyController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CreateLobbyWidget();

		// Retreive the name of the client
		UTemplateGameInstance* gameInstance = Cast<UTemplateGameInstance>(GetGameInstance());
		if (gameInstance)
		{
			_name = gameInstance->playerName;
			AddAControllerToList(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot find the Game Instance !"));
		}
	}
}

void ATemplateLobbyController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATemplateLobbyController, _name);
	DOREPLIFETIME(ATemplateLobbyController, _indexPos);
}

FString ATemplateLobbyController::GetName()
{
	return _name;
}

int ATemplateLobbyController::GetIndex()
{
	return _indexPos;
}

// Call from client
void ATemplateLobbyController::AddAControllerToList(ATemplateLobbyController* newController)
{
	if (Role < ROLE_Authority)
	{
		ServerAddAControllerToList(newController);
	}
	else
	{
		ATemplateLobbyGameMode* GM = Cast<ATemplateLobbyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->AddControllerToList(this);
		}
	}
}

// Server Side
void ATemplateLobbyController::ServerAddAControllerToList_Implementation(ATemplateLobbyController* newController)
{
	AddAControllerToList(newController);
}

bool ATemplateLobbyController::ServerAddAControllerToList_Validate(ATemplateLobbyController* newController)
{
	return true;
}

void ATemplateLobbyController::ClientRefreshHUD_Implementation(FLobbyControllerInfos newController)
{
	RefreshHUD(newController);
}

bool ATemplateLobbyController::ClientRefreshHUD_Validate(FLobbyControllerInfos newController)
{
	return true;
}

void ATemplateLobbyController::ClientAddPendingController_Implementation(FLobbyControllerInfos newController)
{
	AddPendingController(newController);
}

bool ATemplateLobbyController::ClientAddPendingController_Validate(FLobbyControllerInfos newController)
{
	return true;
}

void ATemplateLobbyController::ClientRetreiveController_Implementation(FLobbyControllerInfos newController)
{
	RetreiveController(newController);
}

bool ATemplateLobbyController::ClientRetreiveController_Validate(FLobbyControllerInfos newController)
{
	return true;
}

void ATemplateLobbyController::ClientUpdateController_Implementation(FLobbyControllerInfos newController)
{
	UpdateController(newController);
}

bool ATemplateLobbyController::ClientUpdateController_Validate(FLobbyControllerInfos newController)
{
	return true;
}

void ATemplateLobbyController::SetControllerName(FString newName)
{
	_name = newName;
}

void ATemplateLobbyController::ClientSaveTeam_Implementation(int teamToSave, const FString& theName)
{
	UTemplateGameInstance* gameInstance = Cast<UTemplateGameInstance>(GetGameInstance());
	if (gameInstance)
	{
		gameInstance->SaveTeamNumber(teamToSave);
		gameInstance->SaveName(theName);
	}
}

bool ATemplateLobbyController::ClientSaveTeam_Validate(int teamToSave, const FString& theName)
{
	return true;
}