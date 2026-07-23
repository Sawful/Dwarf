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
	case ORE2: return "Ore";
	case ORE3: return "Ore";
	case ORE4: return "Ore";
	case ORE5: return "Ore";
	}
	return FString();
}

FString GetResourceIcon(ResourceType _type)
{
	return "<img id=\"Resource." + GetResourceName(_type) + "\"/>";
}
