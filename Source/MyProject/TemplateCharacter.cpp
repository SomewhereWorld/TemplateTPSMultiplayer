// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateCharacter.h"
#include "TemplatePlayerState.h"
#include "MyProjectGameMode.h"

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

	GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &ATemplateCharacter::GetPlayerStateAtStart, 1.0f, false);

	_currentPlayerState = EPlayerState::Idle;
	_cameraStart = ThirdPersonCameraComponent->GetRelativeTransform().GetLocation();
	ClientShowVignette(false);

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

}

int ATemplateCharacter::GetTeamNumber()
{
	return _teamNumber;
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
		UE_LOG(LogTemp, Warning, TEXT("Vignette OUI %f"), newWeight);
		UE_LOG(LogTemp, Warning, TEXT("_health %d"), _health);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Vignette NON"));
		PostProcessHurt->BlendWeight = 0.0f;
	}
}

bool ATemplateCharacter::ClientShowVignette_Validate(bool newState)
{
	return true;
}

void ATemplateCharacter::Init()
{
	_health = 100;
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
	_limitVignette = 40.0f;
	_isCrouching = false;
	_isSprinting = false;
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
		//location the PC is focused on
		const FVector Start = ThirdPersonCameraComponent->GetComponentLocation();
		//_fireLength units in facing direction of PC (_fireLength units in front of the Camera)
		const FVector End = Start + (ThirdPersonCameraComponent->GetForwardVector() * _fireLength);

		ServerFire(Start, End);
	}
}

void ATemplateCharacter::ServerFire_Implementation(FVector Start, FVector End)
{
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

	if (!_allDamageSenders.Contains(sender))
	{
		_allDamageSenders.Add(sender);
	}

	UE_LOG(LogTemp, Warning, TEXT("life %d"), _health);

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
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ATemplateCharacter::ClientShowVignette, true);
		//D.BindDynamic(this, &ATemplateCharacter::ClientShowVignette, true);
		GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, RespawnDelegate, 0.05f, false);
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
		GetCharacterMovement()->MaxWalkSpeed = _speedNormal;
		break;
	case EPlayerState::Zooming :
		GetCharacterMovement()->MaxWalkSpeed = _speedWalk;
		break;
	case EPlayerState::Sprinting :
		GetCharacterMovement()->MaxWalkSpeed = _speedSprint;
		break;
	case EPlayerState::Crouching:
		GetCharacterMovement()->MaxWalkSpeed = _speedCrouch;
		break;
	default:
		GetCharacterMovement()->MaxWalkSpeed = _speedNormal;
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

void ATemplateCharacter::DEBUGPROPERTIES()
{
	UE_LOG(LogTemp, Warning, TEXT("life DEBUG %d"), _health);

}
