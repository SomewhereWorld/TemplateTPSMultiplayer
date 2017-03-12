// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TemplateWeapon.h"


// Sets default values
ATemplateWeapon::ATemplateWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
			if (_bulletsInStock > _maxBulletsInMagazine)
			{
				nbToReload = _maxBulletsInMagazine;
			}
			else
			{
				nbToReload = _bulletsInStock;
			}
			_currentBulletsInMagazine = nbToReload;
			_bulletsInStock -= nbToReload;
		}
	}

	if (_wantToFire)
	{
		Fire();
	}
}

void ATemplateWeapon::SetWantToFire(bool newState)
{
	_wantToFire = newState;
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
	_wantToFire = false;
	_canFire = true;
	_currentBulletsInMagazine = _maxBulletsInMagazine;
	_isReloading = false;
}

void ATemplateWeapon::Fire()
{
	if (_canFire && !_isReloading && _currentBulletsInMagazine > 0)
	{
		_canFire = false;
		_currentBulletsInMagazine--;
		// raycast 

		//...

		if (_currentBulletsInMagazine == 0 && _bulletsInStock > 0)
		{
			Reload();
		}
	}
}

void ATemplateWeapon::Reload()
{
	_isReloading = true;
	// launch aniamtion
}