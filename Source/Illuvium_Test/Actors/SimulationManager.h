// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimulationManager.generated.h"

class UMapGenerator;
class ABaseEntity;

USTRUCT(BlueprintType)
struct FEntitiesContainer
{
	GENERATED_BODY()

	int32 Turn;

	ABaseEntity* EntityA;
	ABaseEntity* EntityB;

	FEntitiesContainer() { }

	FEntitiesContainer(int32 InTurn, ABaseEntity* InEntityA, ABaseEntity* InEntityB)
		: Turn(InTurn)
		, EntityA(InEntityA)
		, EntityB(InEntityB) { }

	void UpdateTurn()
	{
		Turn++;
		Turn = Turn % 2;
	}
};

UCLASS()
class ILLUVIUM_TEST_API ASimulationManager : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere)
	UMapGenerator* MapGenerator;

public:
	// Sets default values for this actor's properties
	ASimulationManager();

	UMapGenerator* GetMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetSeed(int32 InSeed);

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void SpawnPawns();

	FVector2D GetMapSize();

private:

	FRandomStream Random;

	UPROPERTY(EditAnywhere)
	int32 Seed;

	UPROPERTY(EditAnywhere)
	int32 MaxAmountOfPlayersByTeam = 1;

	UPROPERTY(EditAnywhere)
	float TimeBetweenMovements = 2.0f;

	UPROPERTY(EditAnywhere)
	FVector2D AttackRange = FVector2D(1.0f, 2.0f);

	UPROPERTY(EditAnywhere)
	FVector2D HealthRange = FVector2D(2.0f, 5.0f);

	UPROPERTY(EditAnywhere)
	FVector2D MapSize = FVector2D(32, 32);

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ABaseEntity>> Entities;

	UFUNCTION()
	void TriggerStep();

	UPROPERTY()
	TArray<ABaseEntity*> EntitiesInGame;

	UPROPERTY()
	TArray<FEntitiesContainer> EntitiesPaired;
};
