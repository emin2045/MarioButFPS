// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackholeEffect.h"

#include "Components/SphereComponent.h"

// Sets default values
ABlackholeEffect::ABlackholeEffect()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionProfileName("NoCollision");
	MeshComp->SetHiddenInGame(true);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(750.f);
	SphereCollision->SetCollisionProfileName("OverlapAllDynamic");
	SphereCollision->SetVisibility(true);
	SphereCollision->SetHiddenInGame(false);

	DefaultStrength = -5000;

}

// Called when the game starts or when spawned
void ABlackholeEffect::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(2.3f);
	UE_LOG(LogTemp, Warning, TEXT("Destroyed"));
	OriginLocation = GetActorLocation();
	OriginLocation.Z = OriginLocation.Z + 200.f;
}

// Called every frame
void ABlackholeEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> OverlappedComps;

	SphereCollision->GetOverlappingComponents(OverlappedComps);


	for (UPrimitiveComponent* Element : OverlappedComps)
	{
		if (Element->ComponentHasTag("PhysicObject"))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s"),*Element->GetName());
			float AppliedStrength = (Element->GetMass() * 5) + DefaultStrength;
			Element->AddRadialForce(OriginLocation, SphereCollision->GetScaledSphereRadius(), AppliedStrength, ERadialImpulseFalloff::RIF_Constant, true);
		}

	}
}

