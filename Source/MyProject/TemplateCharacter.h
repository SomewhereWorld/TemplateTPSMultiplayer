// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "TemplateData.h"
#include "TemplateCharacter.generated.h"

class ATemplatePlayerState;

UCLASS()
class MYPROJECT_API ATemplateCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATemplateCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	ATemplatePlayerState* GetCastedPlayerState();

	int GetTeamNumber();

protected:

	// ****variables****
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	float _fireLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	int _teamNumber;

	// Showing score
	UPROPERTY(BlueprintReadOnly, Category = "Template - Score")
	bool ShowingScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	float _timeToZoom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	float _horizontalAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	float _verticalAcceleration;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	bool _zoomed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	float _speedNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	float _speedSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	float _speedWalk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Camera")
	float _speedCrouch;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	bool _isSprinting;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	bool _isCrouching;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	EPlayerState _currentPlayerState;

	// ****components****
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Template - Player")
	UArrowComponent* FireStart;

	// Third person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Template - Camera")
	UCameraComponent* ThirdPersonCameraComponent;

	// Zoom camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Template - Camera")
	UCameraComponent* ZoomCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Template - Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Template - Player")
	USkeletalMeshComponent* MeshThirdPerson;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Template - Player")
	UPostProcessComponent* PostProcessHurt;

	// ****functions****
	// Refresh the HUD of scores
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshScores();

	// Close the score tab
	UFUNCTION(BlueprintImplementableEvent)
	void HideScores();

	// Show or not the hurt vignette to the player
	UFUNCTION(Reliable, Client, WithValidation)
	void ClientShowVignette(bool newState);
	virtual void ClientShowVignette_Implementation(bool newState);
	virtual bool ClientShowVignette_Validate(bool newState);


private:

	// init all variables
	void Init();

	void MoveForward(float Amount);

	void MoveRight(float Amount);

	void TurnAround(float Amount);

	void LookUp(float Amount);

	// Reset Stats of the player (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerChangeMeshRotation(FRotator rot);
	virtual void ServerChangeMeshRotation_Implementation(const FRotator& rot);
	virtual bool ServerChangeMeshRotation_Validate(const FRotator& rot);

	// Reset Stats of the player (server side)
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastChangeMeshRotation(FRotator rot);
	virtual void MulticastChangeMeshRotation_Implementation(const FRotator& rot);
	virtual bool MulticastChangeMeshRotation_Validate(const FRotator& rot);

	// Handles the Fire on the client
	void Fire();

	// Handles the Fire on the server
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFire(FVector Start, FVector End);
	virtual void ServerFire_Implementation(FVector Start, FVector End);
	virtual bool ServerFire_Validate(FVector Start, FVector End);

	// when the player take damage
	void ReceiveDamage(int Amount, ATemplateCharacter* sender);

	//sets jump flag when key is pressed
	UFUNCTION()
	void OnStartJump();

	//clears jump flag when key is released
	UFUNCTION()
	void OnStopJump();

	// Show scores
	void ShowScores();

	// Stop Show scores
	void StopShowScores();

	// Reset Stats of the player
	void ResetStats();

	// Reset Stats of the player (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerResetStats();
	virtual void ServerResetStats_Implementation();
	virtual bool ServerResetStats_Validate();

	// Set the max speed of the player (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerChangeSpeed();
	virtual void ServerChangeSpeed_Implementation();
	virtual bool ServerChangeSpeed_Validate();

	// Set the new state of the player (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerChangePlayerState(EPlayerState newState);
	virtual void ServerChangePlayerState_Implementation(EPlayerState newState);
	virtual bool ServerChangePlayerState_Validate(EPlayerState newState);

	// Health of the player
	UPROPERTY(Replicated)
	int _health;

	// Damage of the player
	UPROPERTY(Replicated)
	int _damage;

	// where the player is looking at
	UPROPERTY(Replicated)
	float _verticalLook;

	UFUNCTION()
	void GetPlayerStateAtStart();

	FTimerHandle _startTimerHandle;

	// the player state casted
	UPROPERTY(Replicated)
	ATemplatePlayerState* _playerState;

	// list of all players that send damage to us
	TArray<ATemplateCharacter*> _allDamageSenders;

	void Respawn();

	/** ZOOM **/

	void Zoom();

	void UnZoom();

	bool _isZooming;

	// true = zoom forward
	bool _zoomDirection;

	float _zoomValue;

	FVector _cameraStart;
	FVector _cameraEnd;

	// Set the zoomed variable (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerIsZoomed(bool newState);
	virtual void ServerIsZoomed_Implementation(bool newState);
	virtual bool ServerIsZoomed_Validate(bool newState);

	/** ZOOM **/

	void Sprint();
	void StopSprint();

	// Set the sprint variable (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerIsSprinting(bool newState);
	virtual void ServerIsSprinting_Implementation(bool newState);
	virtual bool ServerIsSprinting_Validate(bool newState);

	float _limitVignette;

	void CrouchPlayer();

	void UnCrouchPlayer();

	// Set the sprint variable (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerIsCrouch(bool newState);
	virtual void ServerIsCrouch_Implementation(bool newState);
	virtual bool ServerIsCrouch_Validate(bool newState);

	void DEBUGPROPERTIES();
};