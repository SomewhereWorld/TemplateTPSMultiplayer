// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateCharacter.h"
#include "TemplatePlayerState.h"
#include "MyProjectGameMode.h"
#include "TemplateWeapon.h"

// Sets default values
ATemplateCharacter::ATemplateCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this actor to be replicate.
	bReplicates = true;
	bReplicateMovement = true;

	MeshThirdPerson = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Third Person"));
	MeshThirdPerson->SetupAttachment(GetCapsuleComponent());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());

	ZoomCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Zoom Camera"));
	ZoomCameraComponent->SetupAttachment(SpringArm);

	// Create a CameraComponent
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCameraComponent"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArm);
	// Allow the pawn to control rotation.
	ThirdPersonCameraComponent->bUsePawnControlRotation = true;

	FireStart = CreateDefaultSubobject<UArrowComponent>(TEXT("Fire Start"));
	FireStart->SetupAttachment(ThirdPersonCameraComponent);

	PostProcessHurt = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessHurt"));
	PostProcessHurt->SetupAttachment(GetCapsuleComponent());

	Init();

}

// Called when the game starts or when spawned
void ATemplateCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &ATemplateCharacter::GetPlayerStateAtStart, 0.5f, false);

	_currentPlayerState = EPlayerState::Idle;
	_cameraStart = ThirdPersonCameraComponent->GetRelativeTransform().GetLocation();
	ClientShowVignette(false);

	_weapon = GetWorld()->SpawnActor<ATemplateWeapon>(exempleWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
	_weapon->AttachToComponent(MeshThirdPerson, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));

	_fireLength = _weapon->GetFireLength();
	_damage = _weapon->GetDamage() * _bonusDamageRate;
	//ServerInitWithWeapon(_weapon);
}

// Called every frame
void ATemplateCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (_isZooming)
	{
		if (_zoomDirection)
		{
			_zoomValue += DeltaTime * _timeToZoom;
			if (_zoomValue >= 1.0f)
			{
				_zoomValue = 1.0f;
				_isZooming = false;
				_zoomDirection = !_zoomDirection;
			}
		}
		else
		{
			_zoomValue -= DeltaTime * _timeToZoom;
			if (_zoomValue <= 0)
			{
				_zoomValue = 0.0f;
				_isZooming = false;
				_zoomDirection = !_zoomDirection;
			}
		}
		ThirdPersonCameraComponent->SetRelativeLocation(FMath::Lerp(_cameraStart, _cameraEnd, _zoomValue));

	}

	if (HasAuthority())
	{
		if (_health < _healthMax)
		{
			_currentTimeToRegen += DeltaTime;
			if (_currentTimeToRegen >= _timeToRegen)
			{
				_currentRegen += DeltaTime;
				if (_currentRegen >= _regenSpeed)
				{
					_currentRegen = 0;
					//ServerChangeLife(1);
					_health += 1;
					if (_health > _limitVignette)
					{
						ClientShowVignette(false);
					}
				}
			}
		}
	}

	if (!HasAuthority() && _wantToFire && _weapon)
	{
		if (_weapon->CanFire())
		{
			//location the PC is focused on
			const FVector Start = ThirdPersonCameraComponent->GetComponentLocation();
			//_fireLength units in facing direction of PC (_fireLength units in front of the Camera)
			const FVector End = Start + (ThirdPersonCameraComponent->GetForwardVector() * _fireLength);

			_weapon->Fire();

			ServerFire(Start, End);

			if (_weapon->IsEmpty())
			{
				Reload();
			}
		}
	}
}

// Called to bind functionality to input
void ATemplateCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("Forward", this, &ATemplateCharacter::MoveForward);
	InputComponent->BindAxis("Right", this, &ATemplateCharacter::MoveRight);
	InputComponent->BindAxis("LookUp", this, &ATemplateCharacter::LookUp);
	InputComponent->BindAxis("LookAround", this, &ATemplateCharacter::TurnAround);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATemplateCharacter::Fire);
	InputComponent->BindAction("Fire", IE_Released, this, &ATemplateCharacter::Stopfire);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ATemplateCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ATemplateCharacter::OnStopJump);
	InputComponent->BindAction("ShowScore", IE_Pressed, this, &ATemplateCharacter::ShowScores);
	InputComponent->BindAction("ShowScore", IE_Released, this, &ATemplateCharacter::StopShowScores);
	InputComponent->BindAction("Zoom", IE_Pressed, this, &ATemplateCharacter::Zoom);
	InputComponent->BindAction("Zoom", IE_Released, this, &ATemplateCharacter::UnZoom);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ATemplateCharacter::Sprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ATemplateCharacter::StopSprint);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ATemplateCharacter::CrouchPlayer);
	InputComponent->BindAction("Crouch", IE_Released, this, &ATemplateCharacter::UnCrouchPlayer);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ATemplateCharacter::Reload);

	InputComponent->BindAction("DEBUG", IE_Released, this, &ATemplateCharacter::DEBUGPROPERTIES);
}

void ATemplateCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATemplateCharacter, _health);
	DOREPLIFETIME(ATemplateCharacter, _damage);
	DOREPLIFETIME(ATemplateCharacter, _verticalLook);
	DOREPLIFETIME(ATemplateCharacter, _playerState);
	DOREPLIFETIME(ATemplateCharacter, MeshThirdPerson);
	DOREPLIFETIME(ATemplateCharacter, _zoomed);
	DOREPLIFETIME(ATemplateCharacter, _isSprinting);
	DOREPLIFETIME(ATemplateCharacter, _currentPlayerState);
	DOREPLIFETIME(ATemplateCharacter, _isCrouching);
	DOREPLIFETIME(ATemplateCharacter, _healthMax);
	DOREPLIFETIME(ATemplateCharacter, _weapon);
	DOREPLIFETIME(ATemplateCharacter, _wantToReload);
	DOREPLIFETIME(ATemplateCharacter, _bonusSpeedRate);
	DOREPLIFETIME(ATemplateCharacter, _bonusDamageRate);
	DOREPLIFETIME(ATemplateCharacter, _bonusLifeRate);
}

int ATemplateCharacter::GetTeamNumber()
{
	return _teamNumber;
}

int ATemplateCharacter::GetHealth()
{
	return _health;
}

int ATemplateCharacter::GetMaxHealth()
{
	return _healthMax;
}

bool ATemplateCharacter::GetIsReloading()
{
	return _wantToReload;
}

ATemplatePlayerState* ATemplateCharacter::GetCastedPlayerState()
{
	return _playerState;
}

void ATemplateCharacter::ClientShowVignette_Implementation(bool newState)
{
	if (newState)
	{
		float newWeight = 1.0f - ((float)_health / _limitVignette);
		PostProcessHurt->BlendWeight = newWeight;
	}
	else
	{
		PostProcessHurt->BlendWeight = 0.0f;
	}
}

bool ATemplateCharacter::ClientShowVignette_Validate(bool newState)
{
	return true;
}

void ATemplateCharacter::Init()
{
	_bonusSpeedRate = 1.0f;
	_bonusDamageRate = 1.0f;
	_bonusLifeRate = 1.0f;

	_healthMax = 100;
	_health = _healthMax;
	_damage = 10;
	_fireLength = 5000.0f;
	_isZooming = false;
	_zoomValue = 0;
	_timeToZoom = 4.0f;
	_zoomed = false;
	_horizontalAcceleration = 1.0f;
	_verticalAcceleration = 1.0f;
	_speedNormal = 600.0f;
	_speedWalk = 300.0f;
	_speedSprint = 900.0f;
	_speedCrouch = 150.0f;
	_limitVignette = 50.0f;
	_isCrouching = false;
	_isSprinting = false;
	_timeToRegen = 5.0f;
	_regenSpeed = 0.1f;
	_wantToFire = false;
	_wantToReload = false;
}

void ATemplateCharacter::MoveForward(float Amount)
{
	if (Controller != NULL && Amount != 0.0f)
	{
		if (_currentPlayerState == EPlayerState::Idle || _currentPlayerState == EPlayerState::None)
		{
			_currentPlayerState = EPlayerState::walking;
			ServerChangePlayerState(_currentPlayerState);
		}
		FRotator Rotation = SpringArm->GetComponentRotation();
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			Rotation.Pitch = 0.0f;
		}
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		FRotator toRot = FRotator(GetMesh()->GetComponentRotation().Pitch, SpringArm->GetComponentRotation().Yaw - 90.0f, GetMesh()->GetComponentRotation().Roll);
		ServerChangeMeshRotation(toRot);
		AddMovementInput(Direction, Amount);
	}
}

void ATemplateCharacter::MoveRight(float Amount)
{
	if (Controller != NULL && Amount != 0.0f)
	{
		if (_currentPlayerState == EPlayerState::Idle || _currentPlayerState == EPlayerState::None)
		{
			_currentPlayerState = EPlayerState::walking;
			ServerChangePlayerState(_currentPlayerState);
		}
		FRotator Rotation = SpringArm->GetComponentRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		FRotator toRot = FRotator(GetMesh()->GetComponentRotation().Pitch, SpringArm->GetComponentRotation().Yaw - 90.0f, GetMesh()->GetComponentRotation().Roll);
		ServerChangeMeshRotation(toRot);
		AddMovementInput(Direction, Amount);
	}
}

void ATemplateCharacter::TurnAround(float Amount)
{
	SpringArm->AddWorldRotation(FRotator(0.0f, Amount * _horizontalAcceleration, 0.0f));
	if (_zoomed)
	{
		FRotator toRot = FRotator(GetMesh()->GetComponentRotation().Pitch, SpringArm->GetComponentRotation().Yaw - 90.0f, GetMesh()->GetComponentRotation().Roll);
		ServerChangeMeshRotation(toRot);
	}
}

void ATemplateCharacter::LookUp(float Amount)
{
	if (_verticalLook + (Amount * _verticalAcceleration) <= 89.0f && _verticalLook + (Amount * _verticalAcceleration) >= -89.0f)
	{
		_verticalLook += Amount * _verticalAcceleration;
		ServerChangeVerticalAim(_verticalLook);
		FRotator rot = SpringArm->GetComponentRotation();
		rot.Pitch = _verticalLook;
		SpringArm->SetWorldRotation(FRotator(_verticalLook, rot.Yaw, rot.Roll));
	}
}

void ATemplateCharacter::ATemplateCharacter::ServerChangeMeshRotation_Implementation(const FRotator& rot)
{
	MulticastChangeMeshRotation(rot);
}

bool ATemplateCharacter::ATemplateCharacter::ServerChangeMeshRotation_Validate(const FRotator& rot)
{
	return true;
}

void ATemplateCharacter::MulticastChangeMeshRotation_Implementation(const FRotator& rot)
{
	MeshThirdPerson->SetWorldRotation(rot);
}

bool ATemplateCharacter::MulticastChangeMeshRotation_Validate(const FRotator& rot)
{
	return true;
}

void ATemplateCharacter::Fire()
{
	if (Role < ROLE_Authority)
	{
		_wantToFire = true;
	}
}

void ATemplateCharacter::Stopfire()
{
	_wantToFire = false;
}

void ATemplateCharacter::ServerFire_Implementation(FVector Start, FVector End)
{
	MulticastPlayFireSound();

	FHitResult HitInfo;
	FCollisionQueryParams QParams;
	QParams.AddIgnoredActor(this);
	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;
	FCollisionQueryParams OParams = FCollisionQueryParams::DefaultQueryParam;

	if (GetWorld()->LineTraceSingleByChannel(HitInfo, Start, End, Channel, QParams))
	{
		if (HitInfo.GetActor())
		{
			auto MyPC = Cast<ATemplateCharacter>(HitInfo.GetActor());
			if (MyPC)
			{
				MyPC->ReceiveDamage(_damage, this);
			}
		}
	}

}

bool ATemplateCharacter::ServerFire_Validate(FVector Start, FVector End)
{
	return true;
}

void ATemplateCharacter::ReceiveDamage(int Amount, ATemplateCharacter* sender)
{
	_health -= Amount;
	_currentTimeToRegen = 0;

	if (!_allDamageSenders.Contains(sender))
	{
		_allDamageSenders.Add(sender);
	}

	if (_health <= 0)
	{
		_health = 0;

		if (sender && sender->GetCastedPlayerState())
		{
			sender->GetCastedPlayerState()->AddKill(1);
		}
		if (_playerState)
		{
			_playerState->AddDeath(1);
		}

		for (int i = 0; i < _allDamageSenders.Num(); i++)
		{
			if (_allDamageSenders[i] && _allDamageSenders[i]->GetCastedPlayerState() && _allDamageSenders[i] != sender)
			{
				_allDamageSenders[i]->GetCastedPlayerState()->AddAssist(1);
			}
		}

		_allDamageSenders.Empty();

		Respawn();
	}
	else if (_health <= _limitVignette)
	{
		FTimerDelegate vignetteDelegate = FTimerDelegate::CreateUObject(this, &ATemplateCharacter::ClientShowVignette, true);
		GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, vignetteDelegate, 0.01f, false);
	}
	else
	{
		ClientShowVignette(false);
	}
}

void ATemplateCharacter::OnStartJump()
{
	bPressedJump = true;
}

void ATemplateCharacter::OnStopJump()
{
	bPressedJump = false;
}

void ATemplateCharacter::ShowScores()
{
	ShowingScore = true;
	RefreshScores();
}

void ATemplateCharacter::StopShowScores()
{
	ShowingScore = false;
	HideScores();
}

void ATemplateCharacter::ResetStats()
{
	if (Role == ROLE_Authority)
	{
		_health = 100;
	}
	else if (Role < ROLE_Authority)
	{
		ServerResetStats();
	}
}

void ATemplateCharacter::ServerResetStats_Implementation()
{
	ResetStats();
}

bool ATemplateCharacter::ServerResetStats_Validate()
{
	return true;
}

void ATemplateCharacter::ServerChangeSpeed_Implementation()
{
	switch (_currentPlayerState)
	{
	case EPlayerState::walking:
		GetCharacterMovement()->MaxWalkSpeed = _speedNormal * _bonusSpeedRate;
		break;
	case EPlayerState::Zooming :
		GetCharacterMovement()->MaxWalkSpeed = _speedWalk * _bonusSpeedRate;
		break;
	case EPlayerState::Sprinting :
		GetCharacterMovement()->MaxWalkSpeed = _speedSprint * _bonusSpeedRate;
		break;
	case EPlayerState::Crouching:
		GetCharacterMovement()->MaxWalkSpeed = _speedCrouch * _bonusSpeedRate;
		break;
	default:
		GetCharacterMovement()->MaxWalkSpeed = _speedNormal * _bonusSpeedRate;
		break;
	}
}

bool ATemplateCharacter::ServerChangeSpeed_Validate()
{
	return true;
}

void ATemplateCharacter::ServerChangePlayerState_Implementation(EPlayerState newState)
{
	_currentPlayerState = newState;
}

bool ATemplateCharacter::ServerChangePlayerState_Validate(EPlayerState newState)
{
	return true;
}

void ATemplateCharacter::Respawn()
{
	ResetStats();
	ClientShowVignette(false);
	for (TActorIterator<AMyProjectGameMode> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AMyProjectGameMode* gameMode = Cast<AMyProjectGameMode>(*ActorItr);
		if (gameMode)
		{
			gameMode->ServerRespawn(this);
		}
	}
}

void ATemplateCharacter::GetPlayerStateAtStart()
{
	_playerState = Cast<ATemplatePlayerState>(PlayerState);
}

void ATemplateCharacter::Zoom()
{
	if (_currentPlayerState != EPlayerState::Zooming)
	{
		_currentPlayerState = EPlayerState::Zooming;
		ServerChangePlayerState(_currentPlayerState);
	}

	if (_isSprinting)
	{
		_isSprinting = false;
		ServerIsSprinting(false);
	}

	ServerChangeSpeed();
	_isZooming = true;
	_zoomDirection = true;
	_zoomed = true;
	ServerIsZoomed(true);
	_cameraEnd = ZoomCameraComponent->GetRelativeTransform().GetLocation();
	FRotator toRot = FRotator(GetMesh()->GetComponentRotation().Pitch, SpringArm->GetComponentRotation().Yaw - 90.0f, GetMesh()->GetComponentRotation().Roll);
	ServerChangeMeshRotation(toRot);
}

void ATemplateCharacter::UnZoom()
{
	if (_currentPlayerState == EPlayerState::Zooming)
	{
		_currentPlayerState = EPlayerState::Idle;
		ServerChangePlayerState(_currentPlayerState);
	}
	ServerChangeSpeed();
	_isZooming = true;
	_zoomDirection = false;
	_zoomed = false;
	ServerIsZoomed(false);
}

void ATemplateCharacter::ServerIsZoomed_Implementation(bool newState)
{
	_zoomed = newState;
}

bool ATemplateCharacter::ServerIsZoomed_Validate(bool newState)
{
	return true;
}

void ATemplateCharacter::Sprint()
{
	if (!_zoomed)
	{
		if (_currentPlayerState != EPlayerState::Sprinting)
		{
			_currentPlayerState = EPlayerState::Sprinting;
			ServerChangePlayerState(_currentPlayerState);
		}
		_isSprinting = true;
		ServerIsSprinting(true);
		ServerChangeSpeed();
	}
}

void ATemplateCharacter::StopSprint()
{
	if (_isSprinting)
	{
		if (_currentPlayerState == EPlayerState::Sprinting)
		{
			_currentPlayerState = EPlayerState::Idle;
			ServerChangePlayerState(_currentPlayerState);
		}
		_isSprinting = false;
		ServerIsSprinting(false);
		ServerChangeSpeed();
	}
}

void ATemplateCharacter::ServerIsSprinting_Implementation(bool newState)
{
	_isSprinting = newState;
}

bool ATemplateCharacter::ServerIsSprinting_Validate(bool newState)
{
	return true;
}

void ATemplateCharacter::ServerIsReloading_Implementation(bool newState)
{
	_wantToReload = newState;
}

bool ATemplateCharacter::ServerIsReloading_Validate(bool newState)
{
	return true;
}

void ATemplateCharacter::CrouchPlayer()
{
	if (_currentPlayerState != EPlayerState::Crouching)
	{
		_currentPlayerState = EPlayerState::Crouching;
		ServerChangePlayerState(_currentPlayerState);
	}
	_isCrouching = true;
	ServerIsCrouch(true);
	ServerChangeSpeed();
}

void ATemplateCharacter::UnCrouchPlayer()
{
	if (_currentPlayerState == EPlayerState::Crouching)
	{
		_currentPlayerState = EPlayerState::Idle;
		ServerChangePlayerState(_currentPlayerState);
	}
	_isCrouching = false;
	ServerIsCrouch(false);
	ServerChangeSpeed();
}

void ATemplateCharacter::ServerIsCrouch_Implementation(bool newState)
{
	_isCrouching = newState;
}

bool ATemplateCharacter::ServerIsCrouch_Validate(bool newState)
{
	return true;
}

void ATemplateCharacter::ServerChangeVerticalAim_Implementation(float Amount)
{
	_verticalLook = Amount;
}

bool ATemplateCharacter::ServerChangeVerticalAim_Validate(float Amount)
{
	return true;
}

void ATemplateCharacter::ServerChangeLife_Implementation(float Amount)
{
	_health += Amount;
}

bool ATemplateCharacter::ServerChangeLife_Validate(float Amount)
{
	return true;
}

void ATemplateCharacter::ServerInitWithWeapon_Implementation(ATemplateWeapon* theWeapon)
{
	_fireLength = theWeapon->GetFireLength();
	_damage = theWeapon->GetDamage();
}

bool ATemplateCharacter::ServerInitWithWeapon_Validate(ATemplateWeapon* theWeapon)
{
	return true;
}

void ATemplateCharacter::Reload()
{
	_weapon->Reload();
	ServerChangeWantToReload(true);
}

void ATemplateCharacter::EndReload()
{
	if (HasAuthority())
	{
		ClientEndReload();
	}
}

void ATemplateCharacter::ClientEndReload_Implementation()
{
	_wantToReload = false;
	ServerIsReloading(false);
}

bool ATemplateCharacter::ClientEndReload_Validate()
{
	return true;
}

void ATemplateCharacter::ServerChangeWantToReload_Implementation(bool newState)
{
	_wantToReload = true;
	MulticastPlayReloadSound();
}

bool ATemplateCharacter::ServerChangeWantToReload_Validate(bool newState)
{
	return true;
}

void ATemplateCharacter::MulticastPlayFireSound_Implementation()
{
	_weapon->PlayFireSound();
}

bool ATemplateCharacter::MulticastPlayFireSound_Validate()
{
	return true;
}

void ATemplateCharacter::MulticastPlayReloadSound_Implementation()
{
	_weapon->PlayReloadSound();
}

bool ATemplateCharacter::MulticastPlayReloadSound_Validate()
{
	return true;
}

void ATemplateCharacter::ApplyNewPower()
{
	EPlayerPower lastPowerGet = EPlayerPower::None;
	if (_playerState->GetPower3() != EPlayerPower::None)
	{
		lastPowerGet = _playerState->GetPower3();
	}
	else if (_playerState->GetPower2() != EPlayerPower::None)
	{
		lastPowerGet = _playerState->GetPower2();
	}
	else if (_playerState->GetPower1() != EPlayerPower::None)
	{
		lastPowerGet = _playerState->GetPower1();
	}

	switch (lastPowerGet)
	{
	case EPlayerPower::MarksMan:
		_bonusDamageRate *= 2;
		_damage *= 2;
		break;
	case EPlayerPower::Sprinter:
		_bonusSpeedRate *= 2;
		if (Role < ROLE_Authority)
		{
			ServerChangeSpeed();
		}
		break;
	case EPlayerPower::Tank:
		_bonusLifeRate *= 2;
		_healthMax *= 2;
		_health *= 2;
		break;
	default:
		break;
	}

	if (Role < ROLE_Authority)
	{
		ServerApplyNewPower();
	}
}

void ATemplateCharacter::ServerApplyNewPower_Implementation()
{
	ApplyNewPower();
}

bool ATemplateCharacter::ServerApplyNewPower_Validate()
{
	return true;
}

void ATemplateCharacter::DEBUGPROPERTIES()
{
	UE_LOG(LogTemp, Warning, TEXT("life DEBUG %d"), _health);

}