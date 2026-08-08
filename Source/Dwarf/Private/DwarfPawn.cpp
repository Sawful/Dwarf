// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfPawn.h"
#include "DwarfGameState.h"
#include "Components/Widget.h"
#include "GameFramework/HUD.h"
#include "Cave.h"
#include "DwarfPlayerState.h"

// Sets default values
ADwarfPawn::ADwarfPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Dwarf = CreateDefaultSubobject<USceneComponent>(TEXT("DwarfRoot"));

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetupAttachment(Dwarf);
}

// Called when the game starts or when spawned
void ADwarfPawn::BeginPlay()
{
	Super::BeginPlay();
	SpriteComponent->SetSprite(idle);
	SetActorLocation(pawnOffset);
	isHitting = false;
}

// Called every frame
void ADwarfPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DeltaTime *= TimeFactor;
	if (isHitting)
	{
		hitAnimationClock -= DeltaTime;
		if (hitAnimationClock <= 0)
		{
			SpriteComponent->SetSprite(idle);
			isHitting = false;
		}
	}
	

	// Only walk if block broken
	if (metersWalked >= targetMetersWalked) return;

	FVector newPos = GetActorLocation();
	float progressMade = std::min(DeltaTime * movementSpeed, targetMetersWalked - metersWalked);
	newPos += GetActorRightVector() * progressMade * BLOCK_SIZE;
	metersWalked += progressMade;
	SetActorLocation(newPos);
}

void ADwarfPawn::MoveForward()
{
	targetMetersWalked += 1;
}

void ADwarfPawn::ResetDwarfPawn()
{
	metersWalked = 0;
	targetMetersWalked = 0;
	SetActorLocation(pawnOffset);
}

void ADwarfPawn::PositionToCave(Cave* _cave)
{
	targetMetersWalked = _cave->GetColumnsBroken();
	metersWalked = targetMetersWalked;
	SetActorLocation(pawnOffset + (_cave->first->GetActorLocation() * FVector(1, 1, 0)) - FVector(0, BLOCK_SIZE, 0));
}

void ADwarfPawn::HitAnimation()
{
	SpriteComponent->SetSprite(hit);
	hitAnimationClock = hitAnimationDuration;
	isHitting = true;
}

