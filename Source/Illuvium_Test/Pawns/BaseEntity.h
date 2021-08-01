// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseEntity.generated.h"

class ASimulationManager;
class UTextRenderComponent;

UENUM(BlueprintType)
enum class EEntityState : uint8
{
	IDLE,
	MOVING,
	ATTACKING,
	DYING
};

UCLASS()
class ILLUVIUM_TEST_API ABaseEntity : public APawn
{
	GENERATED_BODY()

private:

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* TextRenderComp;

public:
	// Sets default values for this pawn's properties
	ABaseEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Initialize(int32 Index, ASimulationManager* InSimManager, float InAttackTime, float InHealth);

	void SetHealth(int32 InHealth);

	void ReceiveDamage(int32 InDamage);

	void SetCurrentEnemy(ABaseEntity* InCurrentEnemy);

	// Set current location in coordinates
	void SetCurrentLocation(const FVector2D& InLocation);

	// Set target location in coordinates
	void SetTargetLocation(const FVector2D& InLocation);

	// Get target location in coordinates
	FVector2D GetTargetRawLocation();

	FVector2D Move();

	void Attack();

	UFUNCTION(BlueprintPure)
	float GetSpeed();

	UFUNCTION(BlueprintPure)
	bool GetIsDeath();

	// Is in target location?
	bool GetIsInPlace();

	// Advance until target location
	void SetAdvanceUntilTarget();

private:

	float AttackTime;

	bool bIsInPlace;

	EEntityState CurrentState = EEntityState::IDLE;

	bool bIsDeath, bAdvanceUntilTarget;

	float TimerForMovement;

	int32 TeamIndex;

	int32 Health;

	FVector2D CurrentRawLocation, TargetRawLocation, NextRawLocation;

	FVector CurrentLocation, TargetLocation;

	float Speed;
	
	FVector OldLocation;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> HitMontages;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> DeathMontages;

	UPROPERTY(EditAnywhere)
	FLinearColor TeamAColor = FLinearColor::Blue;

	UPROPERTY(EditAnywhere)
	FLinearColor TeamBColor = FLinearColor::Red;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	void Die();

	UPROPERTY()
	ABaseEntity* CurrentEnemy;

	UPROPERTY()
	ASimulationManager* SimManager;

	void PlayMontage(UAnimMontage* MontageToPlay);
};
