// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEntity.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Actors/SimulationManager.h"
#include "../Components/MapGenerator.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ABaseEntity::ABaseEntity()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = Root;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(RootComponent);

	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComp"));
	TextRenderComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();

	OldLocation = GetActorLocation();
}

// Called every frame
void ABaseEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentEnemy)
	{
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentEnemy->GetActorLocation());
		SetActorRotation(FMath::Lerp(GetActorRotation(), TargetRotation, DeltaTime * 10.0f));
	}

	switch (CurrentState)
	{
	case EEntityState::IDLE:
		break;

	case EEntityState::MOVING:
		TimerForMovement += DeltaTime * 3.0f;
		TimerForMovement = FMath::Clamp(TimerForMovement, 0.0f, 1.0f);
		SetActorLocation(FMath::Lerp(CurrentLocation, TargetLocation, TimerForMovement));
		if (TimerForMovement >= 0.999f)
		{
			TimerForMovement = 0.0f;
			CurrentState = EEntityState::IDLE;
		}
		break;

	case EEntityState::ATTACKING:
		break;

	case EEntityState::DYING:
		break;

	default:
		break;
	}

	Speed = FVector::Dist(GetActorLocation(), OldLocation) / DeltaTime;
	OldLocation = GetActorLocation();
}

// Called to bind functionality to input
void ABaseEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEntity::Initialize(int32 Index, ASimulationManager* InSimManager, float InAttackTime, float InHealth)
{
	TeamIndex = Index;
	AttackTime = InAttackTime;

	SimManager = InSimManager;
	Health = FMath::RoundToInt(InHealth);
	TextRenderComp->SetText(FText::FromString(FString::FromInt(Health)));

	for (int32 i = 0; i < SkeletalMeshComp->GetNumMaterials(); i++)
	{
		DynamicMaterials.Add(SkeletalMeshComp->CreateDynamicMaterialInstance(i));
		if (Index == 0)
		{
			TextRenderComp->SetTextRenderColor(TeamAColor.ToFColor(true));
			DynamicMaterials[i]->SetVectorParameterValue(TEXT("BodyColor"), TeamAColor);
		}
		else
		{
			TextRenderComp->SetTextRenderColor(TeamBColor.ToFColor(true));
			DynamicMaterials[i]->SetVectorParameterValue(TEXT("BodyColor"), TeamBColor);
		}
	}
}

void ABaseEntity::SetHealth(int32 InHealth)
{
	Health = InHealth;
}

void ABaseEntity::ReceiveDamage(int32 InDamage)
{
	if (bIsDeath)
		return;

	Health -= InDamage;
	if (Health <= 0)
	{
		Die();
	}
	else
	{
		TextRenderComp->SetText(FText::FromString(FString::FromInt(Health)));
		PlayMontage(HitMontages[FMath::RandHelper(HitMontages.Num())]);
	}
}

void ABaseEntity::SetCurrentEnemy(ABaseEntity* InCurrentEnemy)
{
	CurrentEnemy = InCurrentEnemy;
}

void ABaseEntity::SetCurrentLocation(const FVector2D& InLocation)
{
	CurrentRawLocation = InLocation;
}

void ABaseEntity::SetTargetLocation(const FVector2D& InLocation)
{
	TargetRawLocation = InLocation;
}

FVector2D ABaseEntity::GetTargetRawLocation()
{
	return TargetRawLocation;
}

FVector2D ABaseEntity::Move()
{
	if (bIsDeath)
		return CurrentRawLocation;

	int32 DeltaX = FMath::Abs(TargetRawLocation.X - CurrentRawLocation.X);
	int32 DeltaY = FMath::Abs(TargetRawLocation.Y - CurrentRawLocation.Y);

	if (DeltaX <= (bAdvanceUntilTarget ? 0 : 1) && DeltaY <= (bAdvanceUntilTarget ? 0 : 1))
	{
		bIsInPlace = true;
		return CurrentRawLocation;
	}

	TimerForMovement = 0.0f;
	CurrentState = EEntityState::MOVING;
	NextRawLocation = CurrentRawLocation;

	if (DeltaX > (bAdvanceUntilTarget ? 0 : 1))
	{
		if (TargetRawLocation.X > CurrentRawLocation.X)
		{
			NextRawLocation.X = CurrentRawLocation.X + 1;
		}
		else
		{
			NextRawLocation.X = CurrentRawLocation.X - 1;
		}

		NextRawLocation.X = FMath::Clamp(NextRawLocation.X, 0.0f, SimManager->GetMapSize().X);
	}
	if (DeltaY > (bAdvanceUntilTarget ? 0 : 1))
	{
		if (TargetRawLocation.Y > CurrentRawLocation.Y)
		{
			NextRawLocation.Y = CurrentRawLocation.Y + 1;
		}
		else
		{
			NextRawLocation.Y = CurrentRawLocation.Y - 1;
		}

		NextRawLocation.Y = FMath::Clamp(NextRawLocation.Y, 0.0f, SimManager->GetMapSize().Y);
	}

	CurrentLocation = SimManager->GetMapGenerator()->GetLocation(CurrentRawLocation.X, CurrentRawLocation.Y);
	TargetLocation = SimManager->GetMapGenerator()->GetLocation(NextRawLocation.X, NextRawLocation.Y);
	CurrentRawLocation = NextRawLocation;

	return NextRawLocation;
}

void ABaseEntity::Attack()
{
	if (!CurrentEnemy)
		return;

	if (bIsDeath || CurrentEnemy->GetIsDeath())
		return;

	CurrentEnemy->ReceiveDamage(1);
	PlayMontage(AttackMontages[FMath::RandHelper(AttackMontages.Num())]);
}

float ABaseEntity::GetSpeed()
{
	return Speed;
}

bool ABaseEntity::GetIsDeath()
{
	return bIsDeath;
}

bool ABaseEntity::GetIsInPlace()
{
	return bIsInPlace;
}

void ABaseEntity::SetAdvanceUntilTarget()
{
	bAdvanceUntilTarget = true;
}

void ABaseEntity::Die()
{
	bIsDeath = true;
	TextRenderComp->SetVisibility(false);
}

void ABaseEntity::PlayMontage(UAnimMontage* MontageToPlay)
{
	SkeletalMeshComp->GetAnimInstance()->Montage_Play(MontageToPlay);
}