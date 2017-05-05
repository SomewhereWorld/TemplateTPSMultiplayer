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

	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	int GetCurrentBulletInMagazine();
	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	int GetBulletInStock();
	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	float GetCurrentReloadingTime();
	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	float GetMaxReloadingTime();
	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	float GetFireLength();
	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	int GetDamage();
	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	bool CanFire();
	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	bool GetIsReloading();

	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Template - Weapon")
	void Reload();

	bool MustReload();

	bool IsEmpty();

	void PlayFireSound();

	void PlayReloadSound();

	void ResetWeapon();

protected:

	// ****components****
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Template - Weapon")
	USkeletalMeshComponent* _skeletalMesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Template - Weapon")
	UAudioComponent* _audioComponent;

	// ****variables****
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	float _fireLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	int _damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	float _fireRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	float _reloadTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	int _maxBulletsInMagazine;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	int _bulletsInStock;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	class USoundBase* _fireSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template - Weapon")
	class USoundBase* _reloadSound;

private:

	float _currentReloadTime;

	float _currentFireRateTime;

	bool _canFire;

	int _currentBulletsInMagazine;

	bool _isReloading;

	void Init();
};
