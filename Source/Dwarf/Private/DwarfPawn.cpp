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
	initialPosition = FVector(0, 0, 92);
}

// Called every frame
void ADwarfPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Only walk if block broken
	if (metersWalked >= targetMetersWalked) return;

	FVector newPos = GetActorLocation();
	float progressMade = std::min(DeltaTime * movementSpeed, targetMetersWalked - metersWalked);
	newPos += GetActorRightVector() * progressMade * BLOCK_SIZE;
	metersWalked += DeltaTime * movementSpeed;
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
	SetActorLocation(initialPosition);
}

void ADwarfPawn::PositionToCave(Cave* _cave)
{
	targetMetersWalked = _cave->GetColumnsBroken();
	metersWalked = targetMetersWalked;
	SetActorLocation(initialPosition + (_cave->first->GetActorLocation() * FVector(1, 1, 0)) - FVector(0, BLOCK_SIZE, 0));
}

