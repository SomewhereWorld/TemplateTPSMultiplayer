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

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(SpringArm);
	// Position the camera a bit above the eyes
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, BaseEyeHeight);
	// Allow the pawn to control rotation.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	FireStart = CreateDefaultSubobject<UArrowComponent>(TEXT("Fire Start"));
	FireStart->SetupAttachment(FirstPersonCameraComponent);

	Init();

}

// Called when the game starts or when spawned
void ATemplateCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(_startTimerHandle, this, &ATemplateCharacter::GetPlayerStateAtStart, 1.0f, false);

}

// Called every frame
void ATemplateCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

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

}

void ATemplateCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATemplateCharacter, _health);
	DOREPLIFETIME(ATemplateCharacter, _damage);
	DOREPLIFETIME(ATemplateCharacter, _verticalLook);
	DOREPLIFETIME(ATemplateCharacter, _playerState);
	DOREPLIFETIME(ATemplateCharacter, MeshThirdPerson);

}

int ATemplateCharacter::GetTeamNumber()
{
	return _teamNumber;
}

ATemplatePlayerState* ATemplateCharacter::GetCastedPlayerState()
{
	return _playerState;
}

void ATemplateCharacter::Init()
{
	_health = 100;
	_damage = 10;
	_fireLength = 5000.0f;
}

void ATemplateCharacter::MoveForward(float Amount)
{
	if (Controller != NULL && Amount != 0.0f)
	{
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
		FRotator Rotation = SpringArm->GetComponentRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Amount);
	}
}

void ATemplateCharacter::TurnAround(float Amount)
{
	SpringArm->AddWorldRotation(FRotator(0.0f, Amount, 0.0f));
}

void ATemplateCharacter::LookUp(float Amount)
{
	if (_verticalLook + Amount <= 89.0f && _verticalLook + Amount >= -89.0f)
	{
		_verticalLook += Amount;
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
		ServerFire();
	}
}

void ATemplateCharacter::ServerFire_Implementation()
{
	//location the PC is focused on
	const FVector Start = FirstPersonCameraComponent ->GetComponentLocation();
	//_fireLength units in facing direction of PC (_fireLength units in front of the Camera)
	const FVector End = Start + (FirstPersonCameraComponent ->GetForwardVector() * _fireLength);
	FHitResult HitInfo;
	FCollisionQueryParams QParams;
	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;
	FCollisionQueryParams OParams = FCollisionQueryParams::DefaultQueryParam;
	if (GetWorld()->LineTraceSingleByChannel(HitInfo, Start, End, ECollisionChannel::ECC_Visibility))
	{
		auto MyPC = Cast<ATemplateCharacter>(HitInfo.GetActor());
		if (MyPC) 
		{
			UE_LOG(LogTemp, Warning, TEXT("%s hits %s"), *PlayerState->PlayerName, *MyPC->PlayerState->PlayerName);
			MyPC->ReceiveDamage(_damage, this);
		}
	}

}

bool ATemplateCharacter::ServerFire_Validate()
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

	if (_health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("life %d"), _health);

		_health = 0;

		if (sender && sender->GetCastedPlayerState())
		{
			sender->GetCastedPlayerState()->AddKill(1);
			UE_LOG(LogTemp, Warning, TEXT("add score"));
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

void ATemplateCharacter::Respawn()
{
	ResetStats();
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