// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

// Sets default values
ABlock::ABlock()
{
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();
}

FString GetResourceName(ResourceType _type)
{
	switch (_type)
	{
	case DIRT: return "Dirt";
	case STONE: return "Stone";
	case ORE: return "Ore";
	}
	return FString();
}
