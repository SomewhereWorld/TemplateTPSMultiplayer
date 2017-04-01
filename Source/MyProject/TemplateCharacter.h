// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "TemplateData.h"
#include "TemplateCharacter.generated.h"

class ATemplatePlayerState;
class ATemplateWeapon;

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

	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	ATemplatePlayerState* GetCastedPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	int GetTeamNumber();

	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	int GetHealth();
	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	int GetMaxHealth();

	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	bool GetIsReloading();

	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	void EndReload();

	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	void ChangePlayerPower(int phase, EPlayerPower newPower);

	void SetClientState(EClientState newState);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Template - Player")
	void LaunchGame();


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
	bool _wantToFire;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	bool _wantToReload;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	bool _isCrouching;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	EPlayerState _currentPlayerState;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	EClientState _currentClientState;

	// where the player is looking at vertically
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Template - Player")
	float _verticalLook;

	// time in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	float _regenSpeed;

	// time before starting to regen after taking a hit(in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	float _timeToRegen;

	// current weapon equipped
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	ATemplateWeapon* _weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Template - Player")
	TSubclassOf<ATemplateWeapon> exempleWeapon;

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

	UFUNCTION(BlueprintCallable, Category = "Template - Player")
	void ApplyNewPower();

	// Set the new power for the player
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerChangePower(int phase, EPlayerPower newPower);
	virtual void ServerChangePower_Implementation(int phase, EPlayerPower newPower);
	virtual bool ServerChangePower_Validate(int phase, EPlayerPower newPower);

	// Refresh the power on the player HUD
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Template - Player")
	void RefreshPowerOnHUD();

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

	void Stopfire();

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

	UPROPERTY(Replicated)
	int _healthMax;

	// Damage of the player
	UPROPERTY(Replicated)
	int _damage;

	// increasing time before starting to regen
	float _currentTimeToRegen;
	
	// increasing time to recover hp
	float _currentRegen;

	UFUNCTION()
	void GetPlayerStateAtStart();

	FTimerHandle _startTimerHandle;

	// the player state casted
	UPROPERTY(Replicated)
	ATemplatePlayerState* _playerState;

	// list of all players that send damage to us
	TArray<ATemplateCharacter*> _allDamageSenders;

	void Respawn();

	// current phase (when a friend die, phase += 1)
	int _phase;

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

	// replicates to all the vertical aim (Server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerChangeVerticalAim(float Amount);
	virtual void ServerChangeVerticalAim_Implementation(float Amount);
	virtual bool ServerChangeVerticalAim_Validate(float Amount);

	// if <0 then lose life
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerChangeLife(float Amount);
	virtual void ServerChangeLife_Implementation(float Amount);
	virtual bool ServerChangeLife_Validate(float Amount);

	// Init player with the weapon variables
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerInitWithWeapon(ATemplateWeapon* theWeapon);
	virtual void ServerInitWithWeapon_Implementation(ATemplateWeapon* theWeapon);
	virtual bool ServerInitWithWeapon_Validate(ATemplateWeapon* theWeapon);

	/** RELOAD **/

	UFUNCTION(Reliable, Client, WithValidation)
	void ClientEndReload();
	virtual void ClientEndReload_Implementation();
	virtual bool ClientEndReload_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerChangeWantToReload(bool newState);
	virtual void ServerChangeWantToReload_Implementation(bool newState);
	virtual bool ServerChangeWantToReload_Validate(bool newState);

	void Reload();

	// Set the reloading variable (server side)
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerIsReloading(bool newState);
	virtual void ServerIsReloading_Implementation(bool newState);
	virtual bool ServerIsReloading_Validate(bool newState);

	/** END RELOAD **/

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastPlayFireSound();
	virtual void MulticastPlayFireSound_Implementation();
	virtual bool MulticastPlayFireSound_Validate();

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastPlayReloadSound();
	virtual void MulticastPlayReloadSound_Implementation();
	virtual bool MulticastPlayReloadSound_Validate();

	UPROPERTY(Replicated)
	float _bonusSpeedRate;

	UPROPERTY(Replicated)
	float _bonusDamageRate;

	UPROPERTY(Replicated)
	float _bonusLifeRate;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerApplyNewPower();
	virtual void ServerApplyNewPower_Implementation();
	virtual bool ServerApplyNewPower_Validate();

	void DistruteOurPower();

	void DEBUGPROPERTIES();
};