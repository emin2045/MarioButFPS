// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackholeProjectile.generated.h"


class ABlackholeEffect;

UCLASS()
class MARIOBUTFPS_API ABlackholeProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABlackholeProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UProjectileMovementComponent* ProjectileMovementComp;

	UFUNCTION()
		void ProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackhole")
		TSubclassOf<ABlackholeEffect> BlackholeEffectClass;

	UPROPERTY(VisibleAnywhere)
		ABlackholeEffect* SpawnedBlackhole;
};
