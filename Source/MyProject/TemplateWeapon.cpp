// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateWeapon.h"


// Sets default values
ATemplateWeapon::ATemplateWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("skeletal mesh"));
	_skeletalMesh->SetupAttachment(RootComponent);

	_audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("audio component"));
	_audioComponent->SetupAttachment(_skeletalMesh);
	_audioComponent->SetSound(_fireSound);

	Init();

}

// Called when the game starts or when spawned
void ATemplateWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATemplateWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (!_canFire)
	{
		_currentFireRateTime += DeltaTime;
		if (_currentFireRateTime >= _fireRate)
		{
			_currentFireRateTime = 0.0f;
			_canFire = true;
		}
	}

	if (_isReloading)
	{
		_currentReloadTime += DeltaTime;
		if (_currentReloadTime >= _reloadTime)
		{
			_currentReloadTime = 0.0f;
			_isReloading = false;
			int nbToReload = 0;
			int remaining = _maxBulletsInMagazine - _currentBulletsInMagazine;
			if (_bulletsInStock > remaining)
			{
				nbToReload = remaining;
			}
			else
			{
				nbToReload = _bulletsInStock;
			}
			_currentBulletsInMagazine += nbToReload;
			_bulletsInStock -= nbToReload;
		}
	}
}

int ATemplateWeapon::GetCurrentBulletInMagazine()
{
	return _currentBulletsInMagazine;
}

int ATemplateWeapon::GetBulletInStock()
{
	return _bulletsInStock;
}

float ATemplateWeapon::GetCurrentReloadingTime()
{
	return _currentReloadTime;
}

float ATemplateWeapon::GetMaxReloadingTime()
{
	return _reloadTime;
}

float ATemplateWeapon::GetFireLength()
{
	return _fireLength;
}

int ATemplateWeapon::GetDamage()
{
	return _damage;
}

bool ATemplateWeapon::CanFire()
{
	if (_canFire && !_isReloading && _currentBulletsInMagazine > 0)
	{
		return true;
	}
	return false;
}

bool ATemplateWeapon::GetIsReloading()
{
	return _isReloading;
}

void ATemplateWeapon::Init()
{
	_fireLength = 5000.0f;
	_damage = 1;
	_fireRate = 0.1f;
	_reloadTime = 5.0f;
	_maxBulletsInMagazine = 25;
	_bulletsInStock = 350;

	_currentReloadTime = 0.0f;
	_currentFireRateTime = 0.0f;
	_canFire = true;
	_currentBulletsInMagazine = _maxBulletsInMagazine;
	_isReloading = false;
}

void ATemplateWeapon::Fire()
{
	if (CanFire())
	{
		_canFire = false;
		_currentBulletsInMagazine--;
	}
}

void ATemplateWeapon::Reload()
{
	_isReloading = true;
}

bool ATemplateWeapon::MustReload()
{
	if (_bulletsInStock > 0 && _currentBulletsInMagazine < _maxBulletsInMagazine)
	{
		return true;
	}
	return false;
}

bool ATemplateWeapon::IsEmpty()
{
	if (_bulletsInStock > 0 && _currentBulletsInMagazine == 0)
	{
		return true;
	}
	return false;
}

void ATemplateWeapon::PlayFireSound()
{
	_audioComponent->SetSound(_fireSound);
	_audioComponent->Play();
}

void ATemplateWeapon::PlayReloadSound()
{
	_audioComponent->SetSound(_reloadSound);
	_audioComponent->Play();
}