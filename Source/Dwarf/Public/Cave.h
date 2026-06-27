// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h" 
#include "Materials/Material.h"
#include "DwarfPlayerState.h" 
#include "Block.h"

#define BLOCK_SIZE 100
#define BLOCK_OFFSET_Y BLOCK_SIZE/2
#define BLOCK_COUNT_X 5
#define BLOCK_COUNT_Y 3

class DWARF_API Cave
{
	int lastGridPos[2];
	ABlock* last;

	void BreakFirst();

	UMaterialInterface* DirtMat;
	UMaterialInterface* StoneMat;
	UMaterialInterface* OreMat;

	TSubclassOf<ABlock> BP_BlockClass = nullptr;

public:
	ABlock* first;
	ADwarfPlayerState* player;

	ABlock* GenerateBlock(FVector _pos);
	void GenerateStart();
	bool DamageFirst(int _damage);

	Cave();
	~Cave();
};
