// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceData.h"
#include "Block.generated.h"

enum BlockType {
	MUDROCK_BLOCK = 0,
	COAL_BLOCK,
	COPPER_BLOCK,
	TIN_BLOCK,
	IRON_BLOCK,
	SULFUR_BLOCK,
	SILVER_BLOCK,
	OBSIDIAN_BLOCK,
	PLATINUM_BLOCK,
	DIAMOND_BLOCK,

	BLOCK_COUNT
};

struct BlockData
{
	BigNumber maxHealth = 50;
	BigNumber health = 50;
	BigNumber expValue = 1;
	BigNumber pressureValue = 0;
	BigNumber pressureRegen = 0;
	int value = 1;
	int index;
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
	void DisconnectFromList();
	void UpdateBreakFactor();

	UStaticMeshComponent* mesh;
	UMaterialInstanceDynamic* mat;
	BlockData Data;
	ABlock* previous = nullptr;
	ABlock* next = nullptr;
	int pos[2];
};

FString GetResourceName(ResourceType _type);
FString GetResourceIcon(ResourceType _type);