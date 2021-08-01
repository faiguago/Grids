// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cells.generated.h"

UCLASS()
class ILLUVIUM_TEST_API ACells : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* InstStaticMeshComp;
	
public:	
	// Sets default values for this actor's properties
	ACells();

	UInstancedStaticMeshComponent* GetInstancedStaticMeshComp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
