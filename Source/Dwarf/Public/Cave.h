// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h" 
#include "Materials/Material.h"
#include "Block.h"

#define BLOCK_SIZE 100
#define BLOCK_OFFSET_Y BLOCK_SIZE/2
#define BLOCK_COUNT_X 5
#define BLOCK_COUNT_Y 4

class ADwarfPawn;

class DWARF_API Cave
{
protected:
	int lastGridPos[2];
	ABlock* last = nullptr;

	void BreakFirst();

	UMaterialInterface* DirtMat;
	UMaterialInterface* StoneMat;
	UMaterialInterface* OreMat;

	TSubclassOf<ABlock> BP_BlockClass = nullptr;

	bool caveVisible = true;
	float blockHealthMult;

	int columnsBroken = 0;
public:
	ABlock* first = nullptr;
	ADwarfPawn* dwarfPawn;

	int GetColumnsBroken() { return columnsBroken; };
	void DestroyCave();
	void ResetCave();
	void SetCaveVisible(bool _visible);
	virtual ABlock* GenerateBlock(FVector _pos);
	void GenerateStart();
	bool DamageFirst(int _damage);

	Cave();
};
