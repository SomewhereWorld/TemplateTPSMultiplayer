// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateLobbyGameMode.h"
#include "TemplateLobbyController.h"

void ATemplateLobbyGameMode::AddControllerToList(ATemplateLobbyController* newController)
{
	if (!isAffecting)
	{
		isAffecting = true;

		if (!_allControllers.Contains(newController))
		{
			// FOR THE TEST
			FString n = newController->GetName();
			int r = FMath::RandRange(0, 20);
			n += FString::FromInt(r);
			newController->SetControllerName(n);
			// FOR THE TEST


			_allControllers.Add(newController);
						
			for (int i = 0; i < _allControllers.Num(); i++)
			{
				if (_allControllers[i] != newController)
				{
					FLobbyControllerInfos controllerInfos;
					controllerInfos.name = newController->GetName();
					_allControllers[i]->ClientRefreshHUD(controllerInfos);
					FLobbyControllerInfos controllerInfos2;
					controllerInfos2.name = _allControllers[i]->GetName();
					newController->ClientRefreshHUD(controllerInfos2);
				}
			}

			FLobbyControllerInfos controllerInfos;
			controllerInfos.name = newController->GetName();
			_allInfos.Add(controllerInfos);
			newController->ClientAddPendingController(controllerInfos);

			isAffecting = false;

			if (_pendingControllers.Num() > 0)
			{
				AddControllerToList(_pendingControllers[0]);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("_allControllers Already contains this Controller ! name = %s"), *newController->GetName());
		}
	}
	else
	{
		_pendingControllers.Add(newController);
	}
}

TArray<ATemplateLobbyController*> ATemplateLobbyGameMode::GetAllController()
{
	return _allControllers;
}

void ATemplateLobbyGameMode::GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATemplateLobbyGameMode, _allControllers);
	DOREPLIFETIME(ATemplateLobbyGameMode, _allInfos);
}

void ATemplateLobbyGameMode::SetNewIndex(FString name, int newIndex)
{
	bool found = false;

	FLobbyControllerInfos info;

	for (int i = 0; i < _allInfos.Num(); i++)
	{
		if (_allInfos[i].name.Equals(name))
		{
			_allInfos[i].indexPos = newIndex;
			found = true;
			info = _allInfos[i];
			break;
		}
	}

	if (found)
	{
		UpdateAllControllers(info);
	}
}

void ATemplateLobbyGameMode::UpdateAllControllers(FLobbyControllerInfos info)
{
	for (int i = 0; i < _allControllers.Num(); i++)
	{
		_allControllers[i]->ClientUpdateController(info);
	}
}