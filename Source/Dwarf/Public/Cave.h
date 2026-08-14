// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h" 

#include "Materials/Material.h"
#include "Engine/TextRenderActor.h"
#include "Engine/StaticMeshActor.h"

#include "Block.h"
#include "DamageSource.h"

#define BLOCK_SIZE 100
#define BLOCK_OFFSET_Y BLOCK_SIZE/2
#define BLOCK_COUNT_X 15
#define BLOCK_COUNT_Y 4
#define FLOOR_LENGTH 25600

class ADwarfPawn;

DECLARE_DELEGATE_OneParam(FOnBlockBreak, BlockData);
class DWARF_API Cave
{
protected:
	int lastGridPos[2];
	int weight[BLOCK_COUNT];
	int caveRank = 0;
	ABlock* last = nullptr;

	ABlock* GetBlock(int _index);
	virtual void BreakFirst();
	virtual void Break(int _index);
	virtual void Break(ABlock* _broken);
	//virtual void Break(int _x, int _y);

	UMaterialInterface* MudrockMat;
	UMaterialInterface* CoalMat;
	UMaterialInterface* CopperMat;
	UMaterialInterface* TinMat;
	UMaterialInterface* IronMat;
	UMaterialInterface* SulfurMat;
	UMaterialInterface* SilverMat;
	UMaterialInterface* ObsidianMat;
	UMaterialInterface* PlatinumMat;
	UMaterialInterface* DiamondMat;
	UMaterialInterface* MagicMat;

	TSubclassOf<ABlock> BP_BlockClass = nullptr;

	bool caveVisible = true;
	float diffBlockHealthMult = 1.0f;
	float distanceHealthMult = 1.0f;

	int columnsBroken = 0;

	const float blockHealthDiffScaling = 2.5f;
	const float blockRewardsDiffScaling = 1.75f;

public:
	bool CheckRank();
	TSubclassOf<AStaticMeshActor> FloorClass = nullptr;
	TSubclassOf <ATextRenderActor> DamageTextClass;
	AStaticMeshActor* repeatableFloors[4];

	AStaticMeshActor* Floor1;
	AStaticMeshActor* Floor2;
	AStaticMeshActor* Roof1;
	AStaticMeshActor* Roof2;

	ABlock* first = nullptr;
	ADwarfPawn* dwarfPawn;
	FOnBlockBreak BlockBreakDelegate;

	int difficultyLevel = 1;
	float yieldMultiplier = 1.0f;

	int blocksGenerated = 0;
	int blocksBroken = 0;

	int GetColumnsBroken() { return columnsBroken; };
	void DestroyCave();
	void ResetCave();
	void SetCaveVisible(bool _visible);
	virtual ABlock* GenerateBlock(FVector _pos);
	void GenerateStart();
	void CreateDamageText(BigNumber _damage, DamageTextType _type, FVector _position);
	void DamageFirst(BigNumber _damage, DamageSource _source);
	void DamageFirstColumn(BigNumber _damage, DamageSource _source);
	void DamageArea(BigNumber _damage, DamageSource _source);
	void DamageRow(BigNumber _damage, DamageSource _source);
	virtual void GenerateTail();
	void SetBlockDataByType(ABlock* _block, BlockType _type);
	void CheckMoveFloor();

	BlockType PickRandomType();

	Cave();
	virtual ~Cave();
};
