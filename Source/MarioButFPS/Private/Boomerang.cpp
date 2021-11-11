// Fill out your copyright notice in the Description page of Project Settings.


#include "Boomerang.h"

#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ABoomerang::OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* OverlappedCharacter = Cast<AShooterCharacter>(OtherActor);

		if (OverlappedCharacter)
		{
			OverlappedCharacter->TakeBoomerang(); // Sets bHasBoomerang true
			GetWorldTimerManager().SetTimerForNextTick(this, &ABoomerang::DestroyBoomerang); // To Prevent Error due to MeshComp
			//Destroy();
		}
	}
}
void ABoomerang::DestroyBoomerang()
{
	Destroy();
}
void ABoomerang::SimulatePhysicHandle()
{
	// Stops playing forward timeline (Deactive) within BP
	GetWorldTimerManager().PauseTimer(TimerHandle_RotateTimer);
	SetActorTickEnabled(false);
	if (MeshComp)
	{
		MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		BoomerangCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		MeshComp->SetSimulatePhysics(true);
	}
}

void ABoomerang::OnBoomerangCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	SimulatePhysicHandle();
}

void ABoomerang::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bCalled)
	{
		FVector NewLocation = MeshComp->GetComponentLocation();
		BoomerangCollision->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		BHitFirst++;
		BHitFirst = UKismetMathLibrary::Clamp(BHitFirst, 0, 2);

		if (BHitFirst > 1)
		{
			// TODO Cam Shake Activated
			bCanBeCalled = true;
		}

	}
}

void ABoomerang::ReturnToInstigator(float Distance)
{
	if (bCanBeCalled)
	{
		bCanBeCalled = false;
		BoomerangCollision->SetWorldLocation(MeshComp->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);

		if (!bCanBeCalled)
		{
			TimelinePlayRate = 1750.f / Distance;
			if (MeshComp)
			{
				MeshComp->SetSimulatePhysics(false);
				MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
			}

			BoomerangCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
			bCalled = true;

			// Set Play Rate of Timeline ( Return Timeline)
			// Play Timeline ( Rising Timeline)
			// Play Timeline (Returning Timeline)

			PlayReturnTimeline();
		}
	}
}

// Sets default values
ABoomerang::ABoomerang()
{
	// Set this actor to call Tick() every frame. 
	PrimaryActorTick.bCanEverTick = true; // Will need for turning back to character

	BoomerangCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BoomerangCollision"));
	RootComponent = BoomerangCollision;
	BoomerangCollision->SetSphereRadius(40.f);
	// Collision Profile Set in BP as custom	


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoomerangCollision);
	MeshComp->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void ABoomerang::BeginPlay()
{
	Super::BeginPlay();

	BoomerangCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoomerang::OnCollisionOverlap);
	BoomerangCollision->OnComponentHit.AddDynamic(this, &ABoomerang::OnBoomerangCollisionHit);
	MeshComp->OnComponentHit.AddDynamic(this, &ABoomerang::OnMeshHit);

	MyInstigator = GetInstigator();

	if (MyInstigator)
	{
		AShooterCharacter* MyCharacter = Cast<AShooterCharacter>(MyInstigator);
		BoomerangCharge = MyCharacter->GetBoomerangCharge();
		SetActorTickEnabled(false);
		const float DeltaSec = UGameplayStatics::GetWorldDeltaSeconds(this);
		GetWorldTimerManager().SetTimer(TimerHandle_RotateTimer, this, &ABoomerang::RotateMesh, DeltaSec, true);
		PlayForwardTimeline();
	}

}

void ABoomerang::RotateMesh()
{
	const FRotator DeltaRotation = FRotator(0, 6.f, 0);
	MeshComp->AddLocalRotation(DeltaRotation);
}

// Called every frame
void ABoomerang::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// At the beginning not ticking.


	const FVector CurrentLocation = GetActorLocation();
	const FVector TargetLocation = MyInstigator->GetActorLocation();
	const FVector NewLocation = UKismetMathLibrary::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 15.f);

	SetActorLocation(NewLocation, true);
}

