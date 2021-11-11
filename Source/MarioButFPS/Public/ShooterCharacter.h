// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UArrowComponent;
class ABlackholeProjectile;
class UCameraShakeBase;

UENUM(BlueprintType)
enum class EFireState : uint8
{

	EFS_MachineGun UMETA(DisplayName = "MachineGun"),
	EFS_HalfLifeGun UMETA(DisplayName = "HalfLifeGun"),
	EFS_Boomerang UMETA(DisplayName = "Boomerang"),

	EFS_NAX UMETA(DisplayName = "DefaultMAX")

};


UCLASS()
class MARIOBUTFPS_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess= "true"))
	USkeletalMeshComponent* ArmMesh;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UArrowComponent* ItemHoldLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UArrowComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		class USoundBase* JumpSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* ArmMesh2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
		bool CameraLocked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float LoveDrunkImpact;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Crouching")
		bool bIsCrouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Sprinting")
		float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Sprinting")
		bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState")
		EFireState FireState;

	UFUNCTION(BlueprintCallable)
		void SetFireState(EFireState NewFireState);

	UFUNCTION(BlueprintCallable)
		void ChangeWeapon1();

	UFUNCTION(BlueprintCallable)
		void ChangeWeapon2();

	UFUNCTION(BlueprintCallable)
		void ChangeWeapon3();

public:

	float MoveForwardAxisValue;
	float MoveRightAxisValue;
	bool PushKeyDown;

	FORCEINLINE EFireState GetFireState() const { return FireState; }

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:

	virtual void BeginPlay() override;

	/*---------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------------------------------------*/
	void LookUp(float AxisValue);
	void Turn(float AxisValue);
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	virtual void Jump() override;
	/*---------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------------------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Movement|Crouching")
		void StartCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement|Crouching")
		void StopCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement|Crouching")
		void UpdateCrouching(float HalfHeight); // runs with timeline in blueprint

		/*---------------------------------------------------------------------------------------------*/
		/*---------------------------------------------------------------------------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Movement|Sprinting")
		void StartSprint();
	UFUNCTION(BlueprintCallable, Category = "Movement|Sprinting")
		void StopSprint();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Movement|Sprinting")
		void SetSprintStateAnim(); // Message via BP Interface and Implemented in BP. It determines Arm Mesh's Anim Play rate while sprinting according to speed of character.

	/*---------------------------------------------------------------------------------------------*/
	/*----------------------------------Blackhole-------------------------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Blackhole")
		void UseBlackhole();

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
		TSubclassOf<ABlackholeProjectile> BlackholeProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Blackhole")
		bool bHasBlackhole;

	UFUNCTION(BlueprintCallable, Category = "Blackhole")
		void SetHasBlackhole();

	/*---------------------------------------------------------------------------------------------*/
	/*-------------------------------Machine Gun --------------------------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Firing")
		void PressedFireButton();

	UFUNCTION(BlueprintCallable, Category = "Firing")
		void ReleasedFireButton();

	UPROPERTY(BlueprintReadOnly, Category = "Firing")
		bool bIsFiring = false;

	UPROPERTY()
		FTimerHandle TimerHandle_FireTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		float FireRate = 0.075f;

	UFUNCTION()
		void StartFire();

	UFUNCTION()
		void LineTraceHandle_MachineGun(FVector& StartLocation, FVector& EndLocation);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing")
		float MachineGunFireRange = 3474;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		float BaseDamage = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		class UParticleSystem* BloodParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		class UParticleSystem* MuzzleParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		TSubclassOf<UCameraShakeBase> FiringCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		class USoundBase* FireSound;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void SetAnimationFiringState(); // Via blueprint interface send message AnimBP

	/*------------------------------------------------------------------------------------------*/
	/*--------------------------------------------Half Life Weapon ----------------------------------------------*/

	UPROPERTY(BlueprintReadOnly, Category = "HalfLifeWeapon")
		FTimerHandle TimerHandle_HLJumpCharge;

	UPROPERTY(BlueprintReadOnly, Category = "HalfLifeWeapon")
		float ChargeAmount = 0.f;

	UFUNCTION(BlueprintCallable, Category = "HalfLifeWeapon")
		void HalfLifeJumpCharger();

	UFUNCTION(BlueprintCallable, Category = "HalfLifeWeapon")
		void ExecuteHLJump();

	UFUNCTION(BlueprintCallable, Category = "HalfLifeWeapon")
		void LineTraceHandle_HLJump(FVector& StartLocation, FVector& EndLocation);

	UPROPERTY(EditDefaultsOnly, Category = "HalfLifeWeapon")
		float HLGunFireRange = 2474;

	UPROPERTY(EditDefaultsOnly, Category = "HalfLifeWeapon")
		class USoundBase* HLFireSound;

	UPROPERTY(EditDefaultsOnly, Category = "HalfLifeWeapon")
		class UParticleSystem* HLGunParticle;

	/*------------------------------------------------------------------------------------------*/
	/*--------------------------------------------Scope----------------------------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Firing")
		void StartAiming();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Firing")
		void AimForward(); // Plays Timeline. Implemented in BP

	UFUNCTION(BlueprintCallable, Category = "Firing")
		void EndAiming();

	UFUNCTION(BlueprintImplementableEvent, Category = "Firing")
		void AimBack(); // Plays Timeline. Implemented in BP

	UFUNCTION(BlueprintCallable, Category = "Firing")
		void UpdateCameraView(float NewField);

	/*------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------------------*/

	UFUNCTION(BlueprintCallable)
		void SetWeaponAppearance(FName WeaponStyle);

	UPROPERTY(EditDefaultsOnly, Category = "CharacterState")
		FLinearColor MachineGunColor; //= FLinearColor(0.5f, 0.26f, 10.f, 1)

	UPROPERTY(EditDefaultsOnly, Category = "CharacterState")
		FLinearColor HalfLifeGunColor; // = FLinearColor(10.f, 3.25f, 0.21f, 1)

	UFUNCTION(BlueprintCallable, Category = "CharacterState")
		void SetArmWeaponVisibility(bool bShouldVisible);

	/*------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------------------*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang")
		float BoomerangCharge = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Boomerang")
		FTimerHandle TimerHandle_BoomerangTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang")
		bool bHasBoomerang = true;

	UFUNCTION(BlueprintCallable)
		void ChargeBoomerang();

	UFUNCTION(BlueprintCallable)
		void ThrowBoomerang();

	UFUNCTION(BlueprintCallable)
		void SpawnBoomerang();

	UPROPERTY(EditDefaultsOnly, Category = "Boomerang")
		TSubclassOf<class ABoomerang> BoomerangClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boomerang")
		class ABoomerang* BoomerangRef; // Last Spawned Boomerang Reference

	UFUNCTION(BlueprintCallable)
		void CallBoomerang();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		float GetBoomerangCharge() { return BoomerangCharge; }

	UFUNCTION(BlueprintCallable)
		void TakeBoomerang();

};


