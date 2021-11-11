// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BlackholeProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Boomerang.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->SetRelativeLocation(FVector(-32.f, 0.f, 50.f));	// Can be changed within Blueprint
	CameraComp->bUsePawnControlRotation = true;

	ArmMesh2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh2->SetupAttachment(CameraComp);
	ArmMesh2->CastShadow = false;
	/*ArmMesh->SetRelativeLocation(FVector(10, 35, -150));
	ArmMesh->SetRelativeRotation(FRotator(0.f, -10.f, 0.f));*/

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(ArmMesh2);
	WeaponMesh->CastShadow = false;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	ItemHoldLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("HoldLocation"));
	ItemHoldLocation->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

	MoveRightAxisValue = 0.f;
	MoveForwardAxisValue = 0.f;

	CameraLocked = false;
	LoveDrunkImpact = 1.f;
	bIsCrouching = false;

	PushKeyDown = false;

	SprintSpeed = 975.f;
	bIsSprinting = false;

	bHasBlackhole = true;

	FireState = EFireState::EFS_MachineGun;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->AttachToComponent(ArmMesh2, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GripPoint"));
	SetWeaponAppearance(FName("MachineGun"));
}

void AShooterCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	const FVector ActorForward = GetActorForwardVector();
	float const ScaleValue = AxisValue * LoveDrunkImpact;
	AddMovementInput(ActorForward, ScaleValue, false);
	MoveForwardAxisValue = ScaleValue;
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	const FVector ActorForward = GetActorRightVector();
	const float ScaleValue = AxisValue * LoveDrunkImpact;
	AddMovementInput(ActorForward, ScaleValue, false);
	MoveForwardAxisValue = ScaleValue;
}

void AShooterCharacter::Turn(float AxisValue)
{
	if (!CameraLocked)
	{
		AddControllerYawInput(AxisValue);
	}
}

void AShooterCharacter::Jump()
{
	const bool bIsFalling = GetCharacterMovement()->IsFalling();
	if (!bIsCrouching && !bIsFalling)
	{
		ACharacter::Jump();
		UGameplayStatics::PlaySound2D(this, JumpSound);
	}
}

void AShooterCharacter::StartCrouch()
{
	bIsCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	// Starts to play timeline in Blueprint
}

void AShooterCharacter::StopCrouch()
{
	bIsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = 650.f;
}

void AShooterCharacter::UpdateCrouching(float HalfHeight) // take float value alpha from lerp node via timeline and then (play & reverse)
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(HalfHeight, true);
}

void AShooterCharacter::StartSprint()
{
	const bool bIsFalling = GetCharacterMovement()->IsFalling();

	if (!bIsFalling && !bIsCrouching && !PushKeyDown)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsSprinting = true;
		AimBack(); // Stop aiming while sprinting
		SetSprintStateAnim();
	}
}

void AShooterCharacter::StopSprint()
{
	const bool bIsFalling = GetCharacterMovement()->IsFalling();

	if (!bIsFalling && !bIsCrouching && !PushKeyDown)
	{
		GetCharacterMovement()->MaxWalkSpeed = 650.f; // Default Walk Speed
		bIsSprinting = false;
		SetSprintStateAnim();
	}
}

void AShooterCharacter::UseBlackhole()
{
	if (bHasBlackhole)
	{
		bHasBlackhole = false;
		const FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
		const FRotator SpawnRotation = CameraComp->GetComponentRotation();
		const FVector SpawnScale = FVector(.2f, .2f, .2f);
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(FQuat(SpawnRotation));
		SpawnTransform.SetScale3D(SpawnScale);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<ABlackholeProjectile>(BlackholeProjectileClass, SpawnTransform, SpawnParams);
		FTimerHandle TimerHandle_SetHasBlackhole;
		GetWorldTimerManager().SetTimer(TimerHandle_SetHasBlackhole, this, &AShooterCharacter::SetHasBlackhole, 2.f, false);
	}
}

void AShooterCharacter::SetHasBlackhole()
{
	bHasBlackhole = true;
}

void AShooterCharacter::PressedFireButton()
{
	if (FireState == EFireState::EFS_MachineGun)
	{
		bIsFiring = true;
		SetAnimationFiringState(); // It uses blueprint interface to communicate with AnimBP... P.S. This function is blueprintimplementable and implemented in BP
		StartFire();
		GetWorldTimerManager().SetTimer(TimerHandle_FireTimerHandle, this, &AShooterCharacter::StartFire, FireRate, true);
	}
	else if (FireState == EFireState::EFS_HalfLifeGun)
	{
		const float DeltaSec = UGameplayStatics::GetWorldDeltaSeconds(this);
		GetWorldTimerManager().SetTimer(TimerHandle_HLJumpCharge, this, &AShooterCharacter::HalfLifeJumpCharger, DeltaSec, true);
	}
	else if (FireState == EFireState::EFS_Boomerang)
	{
		if (bHasBoomerang)
		{
			const float DeltaSec = UGameplayStatics::GetWorldDeltaSeconds(this);
			GetWorldTimerManager().SetTimer(TimerHandle_BoomerangTimer, this, &AShooterCharacter::ChargeBoomerang, DeltaSec, true);
		}
	}
}

void AShooterCharacter::ReleasedFireButton()
{
	if (FireState == EFireState::EFS_MachineGun)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_FireTimerHandle);
		bIsFiring = false;
		SetAnimationFiringState();
	}

	else if (FireState == EFireState::EFS_HalfLifeGun)
	{
		ExecuteHLJump();
	}

	else if (FireState == EFireState::EFS_Boomerang)
	{
		ThrowBoomerang();
	}
}

void AShooterCharacter::ChargeBoomerang()
{
	BoomerangCharge += 0.04f;
	BoomerangCharge = FMath::Clamp(BoomerangCharge, 1.f, 3.f);
}

void AShooterCharacter::ThrowBoomerang()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BoomerangTimer);
	SpawnBoomerang();
	BoomerangCharge = 1.f;
}

void AShooterCharacter::SpawnBoomerang()
{
	if (bHasBoomerang)
	{
		const FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
		const FRotator SpawnRotation = CameraComp->GetComponentRotation();
		const FVector SpawnScale = FVector(0.3f);
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(FQuat(SpawnRotation));
		SpawnTransform.SetScale3D(SpawnScale);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		BoomerangRef = GetWorld()->SpawnActor<ABoomerang>(BoomerangClass, SpawnTransform, SpawnParams);
		bHasBoomerang = false;
	}
}

void AShooterCharacter::CallBoomerang()
{
	if (BoomerangRef)
	{
		float Distance;
		FVector V1 = GetActorLocation();
		FVector V2 = BoomerangRef->GetActorLocation();

		Distance = UKismetMathLibrary::Vector_Distance(V1, V2);

		BoomerangRef->ReturnToInstigator(Distance);
	}
}

void AShooterCharacter::TakeBoomerang()
{
	bHasBoomerang = true;
}


void AShooterCharacter::HalfLifeJumpCharger()
{
	ChargeAmount += 20.f;
	ChargeAmount = FMath::Clamp(ChargeAmount, 0.f, 1500.f);

}

void AShooterCharacter::ExecuteHLJump()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HLJumpCharge);

	UGameplayStatics::SpawnSound2D(this, HLFireSound);

	FHitResult OutHit;
	FVector TraceStart;
	FVector TraceEnd;

	LineTraceHandle_HLJump(TraceStart, TraceEnd); // Determine where line trace starts and ends

	if (GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility) && ChargeAmount > 300.f)
	{
		const FVector LaunchVelocity = CameraComp->GetForwardVector() * -1 * ChargeAmount;
		LaunchCharacter(LaunchVelocity, true, true);

		const FTransform MuzzleTransform = WeaponMesh->GetSocketTransform(FName("MuzzleSocket"));
		auto TrailParticle = UGameplayStatics::SpawnEmitterAtLocation(this, HLGunParticle, MuzzleTransform.GetLocation(), FRotator(MuzzleTransform.GetRotation()), MuzzleTransform.GetScale3D());
		TrailParticle->SetVectorParameter(FName("Target"), OutHit.ImpactPoint); // trail from the muzzle of the gun to where it hits
	}

	ChargeAmount = 0.f; // Reset Charge amount to be ready next HL Jump
	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, WeaponMesh, FName("MuzzleSocket"));

}

void AShooterCharacter::StartFire()
{
	FHitResult OutHit;
	FVector StartLocation; // Camera World Location
	FVector TraceEnd;
	LineTraceHandle_MachineGun(StartLocation, TraceEnd);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, TraceEnd, ECollisionChannel::ECC_Visibility);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *OutHit.Actor->GetName());
		AActor* HitActor = OutHit.GetActor();
		if (HitActor && HitActor->ActorHasTag(FName("CanBeDamaged")))
		{
			if (OutHit.BoneName == "head")
			{
				UGameplayStatics::ApplyDamage(HitActor, BaseDamage * 3, GetController(), this, nullptr);
			}
			else
			{
				UGameplayStatics::ApplyDamage(HitActor, BaseDamage, GetController(), this, nullptr);
			}

			const float RandomScale = UKismetMathLibrary::RandomFloatInRange(.5f, .65f);
			const FVector EmitterScale = FVector(RandomScale);
			UGameplayStatics::SpawnEmitterAtLocation(this, BloodParticle, OutHit.ImpactPoint, FRotator::ZeroRotator, EmitterScale);
		}
		UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, WeaponMesh, FName("MuzzleSocket"));
		UGameplayStatics::GetPlayerController(this, 0)->ClientStartCameraShake(FiringCameraShake);
		UGameplayStatics::SpawnSoundAtLocation(this, FireSound, this->GetActorLocation());
	}
}

void AShooterCharacter::LineTraceHandle_MachineGun(FVector& StartLocation, FVector& EndLocation)
{
	StartLocation = CameraComp->GetComponentLocation();

	EndLocation = StartLocation + (CameraComp->GetForwardVector() * MachineGunFireRange);
}

void AShooterCharacter::LineTraceHandle_HLJump(FVector& StartLocation, FVector& EndLocation)
{
	StartLocation = CameraComp->GetComponentLocation();

	EndLocation = StartLocation + (CameraComp->GetForwardVector() * HLGunFireRange);
}

//Default 1-2-3 Key to change FireState
void AShooterCharacter::ChangeWeapon1()
{
	SetFireState(EFireState::EFS_MachineGun);
	ArmMesh2->SetVisibility(true, true);
	SetWeaponAppearance(FName("MachineGun"));

}

void AShooterCharacter::ChangeWeapon2()
{
	SetFireState(EFireState::EFS_HalfLifeGun);
	ArmMesh2->SetVisibility(true, true);
	AimBack();
	SetWeaponAppearance(FName("HalfLifeGun"));
}

void AShooterCharacter::ChangeWeapon3()
{
	SetFireState(EFireState::EFS_Boomerang);
	ArmMesh2->SetVisibility(false, true);
	AimBack();
}

void AShooterCharacter::SetFireState(EFireState NewFireState)
{
	FireState = NewFireState;
}

void AShooterCharacter::StartAiming()
{
	if (FireState == EFireState::EFS_MachineGun)
	{
		AimForward();
	}
}

void AShooterCharacter::EndAiming()
{
	AimBack();
}

void AShooterCharacter::UpdateCameraView(float NewField) // For Aiming Increase or Decrease Field of View
{
	CameraComp->SetFieldOfView(NewField);
}

void AShooterCharacter::SetWeaponAppearance(FName WeaponStyle)
{
	if (WeaponStyle == FName("MachineGun"))
	{
		WeaponMesh->CreateDynamicMaterialInstance(1)->SetVectorParameterValue(FName("Color"), MachineGunColor);
		WeaponMesh->CreateDynamicMaterialInstance(0)->SetVectorParameterValue(FName("Color"), MachineGunColor);
	}
	else
	{
		WeaponMesh->CreateDynamicMaterialInstance(1)->SetVectorParameterValue(FName("Color"), HalfLifeGunColor);
		WeaponMesh->CreateDynamicMaterialInstance(0)->SetVectorParameterValue(FName("Color"), HalfLifeGunColor);
	}
}

void AShooterCharacter::SetArmWeaponVisibility(bool bShouldVisible)
{
	ArmMesh2->SetVisibility(bShouldVisible, true);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShooterCharacter::StopCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AShooterCharacter::StopSprint);

	PlayerInputComponent->BindAction("UseBlackhole", IE_Pressed, this, &AShooterCharacter::UseBlackhole);

	PlayerInputComponent->BindAction("ChangeFireAbility_1", IE_Pressed, this, &AShooterCharacter::ChangeWeapon1);
	PlayerInputComponent->BindAction("ChangeFireAbility_2", IE_Pressed, this, &AShooterCharacter::ChangeWeapon2);
	PlayerInputComponent->BindAction("ChangeFireAbility_3", IE_Pressed, this, &AShooterCharacter::ChangeWeapon3);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::PressedFireButton);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::ReleasedFireButton);

	PlayerInputComponent->BindAction("Scope", IE_Pressed, this, &AShooterCharacter::StartAiming);
	PlayerInputComponent->BindAction("Scope", IE_Released, this, &AShooterCharacter::EndAiming);

	PlayerInputComponent->BindAction("CallBoomerang", IE_Pressed, this, &AShooterCharacter::CallBoomerang);


	//PlayerInputComponent->BindAction("ChangeFireAbility_4", IE_Pressed, this, &AShooterCharacter::BeginPlay);

	/*PlayerInputComponent->BindAction("DropObject", IE_Pressed, this, &AShooterCharacter::BeginPlay);
	PlayerInputComponent->BindAction("HealthSupport", IE_Pressed, this, &AShooterCharacter::BeginPlay);
	PlayerInputComponent->BindAction("OpenDoor", IE_Pressed, this, &AShooterCharacter::BeginPlay);
	PlayerInputComponent->BindAction("PushObject", IE_Pressed, this, &AShooterCharacter::BeginPlay);*/
}