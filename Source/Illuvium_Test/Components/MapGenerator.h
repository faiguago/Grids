// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapGenerator.generated.h"

class ACells;

USTRUCT(BlueprintType)
struct FCellContainer
{
	GENERATED_BODY()

	int32 Row;
	int32 Column;
	
	bool bIsEmpty;
	FVector Location;

	FCellContainer() : bIsEmpty(true) { }

	FCellContainer(const int32& InRow, const int32& InColumn, const FVector& InLocation)
		: Row(InRow)
		, Column(InColumn)
		, bIsEmpty(true)
		, Location(InLocation) { }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ILLUVIUM_TEST_API UMapGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapGenerator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACells> CellsPrefab;

	void GenerateMap(const FVector2D& InMapSize);
	
	TArray<FCellContainer> Cells;

	// Get location in World Space
	FVector GetLocation(int32 Row, int32 Column);

	// Repaint cell with specific index
	void UpdateCellState(bool bInIsEmpty, int32 Index, bool bInIsMovement = true);

	// Clear all cells (white color)
	void MarkAllCellsAsEmpty();

private:

	FVector2D MapSize;

	UPROPERTY()
	ACells* CellsActor;
};
