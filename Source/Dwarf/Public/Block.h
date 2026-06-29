// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceData.h"
#include "Block.generated.h"

enum BlockType {
	DIRT_BLOCK = 0,
	STONE_BLOCK = 1,
	ORE_BLOCK = 2,

	BLOCK_COUNT
};

struct BlockData
{
	int health = 50;
	BlockType type;
	TArray<ResourceData> yield;
};

UCLASS()
class DWARF_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UStaticMeshComponent* mesh;
	BlockData Data;
	ABlock* next = nullptr;
};

FString GetResourceName(ResourceType _type);