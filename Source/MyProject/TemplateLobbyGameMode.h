// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "TemplateData.h"
#include "TemplateLobbyGameMode.generated.h"

class ATemplateLobbyController;

/**
 * 
 */
UCLASS()
class MYPROJECT_API ATemplateLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	void AddControllerToList(ATemplateLobbyController* newController);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const override;

	// Server Side
	TArray<ATemplateLobbyController*> GetAllController();

	UFUNCTION(BlueprintCallable, Category = "Template - Lobby Game Mode")
	void SetNewIndex(FString name, int newIndex);

private:

	// Server Side
	UPROPERTY(replicated)
	TArray<ATemplateLobbyController*> _allControllers;

	UPROPERTY(replicated)
	TArray<FLobbyControllerInfos> _allInfos;

	bool isAffecting;
	TArray<ATemplateLobbyController*> _pendingControllers;
	
	void UpdateAllControllers(FLobbyControllerInfos info);
};
