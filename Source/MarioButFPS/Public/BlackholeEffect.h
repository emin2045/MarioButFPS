// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackholeEffect.generated.h"

UCLASS()
class MARIOBUTFPS_API ABlackholeEffect : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABlackholeEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector OriginLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float DefaultStrength;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
