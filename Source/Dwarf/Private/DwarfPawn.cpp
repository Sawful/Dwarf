// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfPawn.h"
#include "DwarfGameState.h"
#include "Components/Widget.h"
#include "GameFramework/HUD.h"
#include "Cave.h"

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
	initialPosition = GetActorLocation();
}

// Called every frame
void ADwarfPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Only walk if block broken
	if (metersWalked >= targetMetersWalked) return;

	FVector newPos = GetActorLocation();
	newPos += GetActorRightVector() * DeltaTime * movementSpeed * BLOCK_SIZE;
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

