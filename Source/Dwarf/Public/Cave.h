// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h" 
#include "Materials/Material.h"
#include "Block.h"

#define BLOCK_SIZE 100
#define BLOCK_OFFSET_Y BLOCK_SIZE/2
#define BLOCK_COUNT_X 5
#define BLOCK_COUNT_Y 3

class ADwarfPlayerState;

class DWARF_API Cave
{
protected:
	int lastGridPos[2];
	ABlock* last;

	void BreakFirst();

	UMaterialInterface* DirtMat;
	UMaterialInterface* StoneMat;
	UMaterialInterface* OreMat;

	TSubclassOf<ABlock> BP_BlockClass = nullptr;

	bool caveVisible = true;

public:
	ABlock* first;
	ADwarfPlayerState* player;

	void ResetCave();
	void SetCaveVisible(bool _visible);
	virtual ABlock* GenerateBlock(FVector _pos);
	void GenerateStart();
	bool DamageFirst(int _damage);

	Cave();
};
