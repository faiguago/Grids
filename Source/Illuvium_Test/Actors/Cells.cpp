// Fill out your copyright notice in the Description page of Project Settings.


#include "Cells.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
ACells::ACells()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = Root;

	InstStaticMeshComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComp"));
	InstStaticMeshComp->SetupAttachment(RootComponent);
}

UInstancedStaticMeshComponent* ACells::GetInstancedStaticMeshComp()
{
	return InstStaticMeshComp;
}

// Called when the game starts or when spawned
void ACells::BeginPlay()
{
	Super::BeginPlay();
	
}