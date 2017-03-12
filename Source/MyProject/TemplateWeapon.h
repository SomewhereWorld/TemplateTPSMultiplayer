// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TemplateWeapon.generated.h"

UCLASS()
class MYPROJECT_API ATemplateWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATemplateWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetWantToFire(bool newState);

	int GetCurrentBulletInMagazine();
	int GetBulletInStock();
	float GetCurrentReloadingTime();

protected:

	// ****variables****
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	float _fireLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	int _damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	float _fireRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	float _reloadTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	int _maxBulletsInMagazine;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Player")
	int _bulletsInStock;

private:

	float _currentReloadTime;

	float _currentFireRateTime;
	
	bool _wantToFire;

	bool _canFire;

	int _currentBulletsInMagazine;

	bool _isReloading;


	void Init();

	void Fire();

	void Reload();
};
