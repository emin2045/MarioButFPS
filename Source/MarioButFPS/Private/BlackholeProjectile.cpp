// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackholeProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "BlackholeEffect.h"

// Sets default values
ABlackholeProjectile::ABlackholeProjectile()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	RootComponent = MeshComp;

	MeshComp->SetCollisionProfileName("BlockAll");

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 2200.f;
	ProjectileMovementComp->MaxSpeed = 2200.f;
}

// Called when the game starts or when spawned
void ABlackholeProjectile::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->OnComponentHit.AddDynamic(this, &ABlackholeProjectile::ProjectileHit);
}

void ABlackholeProjectile::ProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector SpawnLocation = Hit.Location;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ABlackholeEffect>(BlackholeEffectClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	Destroy();
}