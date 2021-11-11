// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boomerang.generated.h"

UCLASS()
class MARIOBUTFPS_API ABoomerang : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USphereComponent* BoomerangCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BoomerangCharge;

	UFUNCTION(BlueprintCallable)
		void RotateMesh();

	UPROPERTY(BlueprintReadWrite)
		FTimerHandle TimerHandle_RotateTimer;

	UPROPERTY(BlueprintReadOnly)
		APawn* MyInstigator;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void PlayForwardTimeline(); // Implemented in BP to play Timeline

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float BoomerangRangeMultiplier = 1.6f;

	UFUNCTION(BlueprintCallable)
		void OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void SimulatePhysicHandle();

	UFUNCTION(BlueprintCallable)
		void OnBoomerangCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
		void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(BlueprintReadWrite)
		bool bCalled = false; // is Called by Instigator?

	UPROPERTY(BlueprintReadWrite)
		int32 BHitFirst = 0;

	UPROPERTY(BlueprintReadWrite)
		bool bCanBeCalled = false;

	UPROPERTY(BlueprintReadWrite)
		float TimelinePlayRate;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void PlayReturnTimeline();

	UFUNCTION(BlueprintCallable)
		void DestroyBoomerang();

public:
	// Sets default values for this actor's properties
	ABoomerang();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void ReturnToInstigator(float Distance);
};
