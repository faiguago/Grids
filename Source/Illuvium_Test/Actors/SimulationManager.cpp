// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationManager.h"
#include "../Components/MapGenerator.h"
#include "../Pawns/BaseEntity.h"

// Sets default values
ASimulationManager::ASimulationManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MapGenerator = CreateDefaultSubobject<UMapGenerator>("MapGeneratorComp");
	AddOwnedComponent(MapGenerator);
}

UMapGenerator* ASimulationManager::GetMapGenerator()
{
	return MapGenerator;
}

// Called when the game starts or when spawned
void ASimulationManager::BeginPlay()
{
	Super::BeginPlay();

	// Dummy Initialization
	Initialize();

	// Dummy Trigger Next Step
	// TODO: trigger from sim (fixed tick)
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASimulationManager::TriggerStep, TimeBetweenMovements, true);
}

void ASimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ASimulationManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

// Called every frame
void ASimulationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASimulationManager::SetSeed(int32 InSeed)
{
	Seed = InSeed;
}

void ASimulationManager::Initialize()
{
	// Deterministic simulation
	Random = FRandomStream(Seed);

	// Generate map and spawn pawns
	MapGenerator->GenerateMap(MapSize);
	SpawnPawns();
}

void ASimulationManager::SpawnPawns()
{
	UWorld* const World = GetWorld();
	if (!World)
		return;

	// Spawn all the entities
	for (int32 i = 0; i < MaxAmountOfPlayersByTeam; i++)
	{
		TArray<FVector2D> Coordinates;
		TArray<ABaseEntity*> EntitiesTemp;

		// Spawn entity for each team
		for (int32 j = 0; j < 2; j++)
		{
			// Random Entity
			int32 EntityIndex = Random.RandRange(0, Entities.Num() - 1);

			int32 Row = Random.RandRange(0, MapSize.X - 1);
			int32 Column = Random.RandRange(0, MapSize.Y - 1);
			Coordinates.Add(FVector2D(Row, Column));

			float Health = Random.FRandRange(HealthRange.X, HealthRange.Y);
			float AttackTime = Random.FRandRange(AttackRange.X, AttackRange.Y);

			FVector Location = MapGenerator->GetLocation(Row, Column);
			ABaseEntity* EntityActor = World->SpawnActor<ABaseEntity>(Entities[EntityIndex], Location, FRotator::ZeroRotator);
			EntityActor->Initialize(j, this, AttackTime, Health);
			EntityActor->SetCurrentLocation(FVector2D(Row, Column));

			EntitiesTemp.Add(EntityActor);
			EntitiesInGame.Add(EntityActor);

			int32 Index = Row + Column * MapSize.Y;
			MapGenerator->UpdateCellState(false, Index);
		}

		// Set current enemy;
		EntitiesTemp[0]->SetCurrentEnemy(EntitiesTemp[1]);
		EntitiesTemp[1]->SetCurrentEnemy(EntitiesTemp[0]);
		EntitiesPaired.Add(FEntitiesContainer(Random.RandHelper(2), EntitiesTemp[0], EntitiesTemp[1]));

		// Set advance until target
		Random.RandHelper(2) == 0 ? EntitiesTemp[0]->SetAdvanceUntilTarget() : EntitiesTemp[1]->SetAdvanceUntilTarget();

		// Calculate target coordinate
		FVector2D TargetCoordinate(FMath::RoundToInt((Coordinates[0].X + Coordinates[1].X) / 2), FMath::RoundToInt((Coordinates[0].Y + Coordinates[1].Y) / 2));
		MapGenerator->UpdateCellState(false, TargetCoordinate.X + TargetCoordinate.Y * MapSize.Y, false);

		// Send target coordinate
		EntitiesTemp[0]->SetTargetLocation(TargetCoordinate);
		EntitiesTemp[1]->SetTargetLocation(TargetCoordinate);
	}
}

FVector2D ASimulationManager::GetMapSize()
{
	return MapSize;
}

void ASimulationManager::TriggerStep()
{
	// Containers for locations
	TArray<FVector2D> CurrentLocations;
	TArray<FVector2D> TargetLocations;

	// Move entities
	for(int32 i = 0; i < EntitiesInGame.Num(); i++)
	{
		CurrentLocations.Add(EntitiesInGame[i]->Move());
		TargetLocations.Add(EntitiesInGame[i]->GetTargetRawLocation());
	}

	// Repaint cells
	MapGenerator->MarkAllCellsAsEmpty();
	for(int32 i = 0; i < CurrentLocations.Num(); i++)
	{
		MapGenerator->UpdateCellState(false, CurrentLocations[i].X + CurrentLocations[i].Y * MapSize.Y);
	}
	for (int32 i = 0; i < TargetLocations.Num(); i++)
	{
		MapGenerator->UpdateCellState(false, TargetLocations[i].X + TargetLocations[i].Y * MapSize.Y, false);
	}

	// Attack 
	for(int32 i = 0; i < EntitiesPaired.Num(); i++)
	{
		if(EntitiesPaired[i].EntityA->GetIsInPlace() && EntitiesPaired[i].EntityB->GetIsInPlace())
		{
			if(EntitiesPaired[i].Turn == 0)
			{
				EntitiesPaired[i].EntityA->Attack();
			}
			else
			{
				EntitiesPaired[i].EntityB->Attack();
			}

			EntitiesPaired[i].UpdateTurn();
		}
	}
}