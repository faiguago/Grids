// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"
#include "../Actors/Cells.h"

// Sets default values for this component's properties
UMapGenerator::UMapGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UMapGenerator::GenerateMap(const FVector2D& InMapSize)
{
	UWorld* const World = GetWorld();
	if (!World)
		return;

	// Spawn cells actor
	MapSize = InMapSize;
	CellsActor = World->SpawnActor<ACells>(CellsPrefab, FVector::ZeroVector, FRotator::ZeroRotator);

	// Spawn all the instances and set the custom data value
	for (int32 x = 0; x < MapSize.X; x++)
	{
		for (int32 y = 0; y < MapSize.Y; y++)
		{
			int32 Index = x + y;
			FVector Location = FVector(x * 100.0f, y * 100.0f, 0.0f);
			Cells.Add(FCellContainer(x, y, Location));
			CellsActor->GetInstancedStaticMeshComp()->AddInstance(FTransform(FQuat::Identity, Location, FVector::OneVector * 0.9f));
			CellsActor->GetInstancedStaticMeshComp()->SetCustomDataValue(x * y + y, 0, 0.0f, true);
		}
	}
}

FVector UMapGenerator::GetLocation(int32 Row, int32 Column)
{
	int32 Index = Row + Column * MapSize.Y;

	if (Cells.IsValidIndex(Index))
		return Cells[Index].Location;
	else
		return FVector::ZeroVector;
}

void UMapGenerator::UpdateCellState(bool bInIsEmpty, int32 Index, bool bInIsMovement)
{
	if (!Cells.IsValidIndex(Index))
		return;

	Cells[Index].bIsEmpty = bInIsEmpty;
	float NewVal = bInIsEmpty ? 0.0f : 1.0f;
	if (bInIsMovement)
	{
		// Update cell color for current coordinate
		CellsActor->GetInstancedStaticMeshComp()->SetCustomDataValue(Index, 0, NewVal, true);
	}
	else
	{
		// Update cell color for target coordinate
		CellsActor->GetInstancedStaticMeshComp()->SetCustomDataValue(Index, 1, NewVal, true);
	}
}

void UMapGenerator::MarkAllCellsAsEmpty()
{
	for (int32 i = 0; i < Cells.Num(); i++)
	{
		Cells[i].bIsEmpty = true;
		CellsActor->GetInstancedStaticMeshComp()->SetCustomDataValue(i, 0, 0.0f, true);
		CellsActor->GetInstancedStaticMeshComp()->SetCustomDataValue(i, 1, 0.0f, true);
	}
}
