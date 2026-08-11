// Fill out your copyright notice in the Description page of Project Settings.


#include "Cave.h"
#include "DwarfPawn.h"
#include "Engine/StaticMeshActor.h"

void Cave::GenerateStart()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GEngine->AddOnScreenDebugMessage(0, 10, FColor::Emerald, FString::FromInt(IsValid(FloorClass)));

	repeatableFloors[0] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, FLOOR_LENGTH, 800), FRotator(), param);
	repeatableFloors[1] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, 0, 800), FRotator(), param);
	repeatableFloors[2] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, FLOOR_LENGTH, -400), FRotator(), param);
	repeatableFloors[3] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, 0, -400), FRotator(), param);

	diffBlockHealthMult = powf(blockHealthDiffScaling, difficultyLevel);
	yieldMultiplier = powf(blockRewardsDiffScaling, difficultyLevel);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Tried generating cave")));
	if (BP_BlockClass == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Could not create block class.")));
		return;
	}

	lastGridPos[0] = 1;
	lastGridPos[1] = blocksGenerated%4;
	first = GenerateBlock(FVector(0, lastGridPos[0] * BLOCK_SIZE, (BLOCK_COUNT_Y - 1 - lastGridPos[1]) * BLOCK_SIZE + BLOCK_OFFSET_Y));
	last = first;
	int j = 1;

	for (int i = 0; i < BLOCK_COUNT_X * BLOCK_COUNT_Y - 1; i++)
	{
		GenerateTail();
	}
}

void Cave::DestroyCave()
{
	if (!first) return;
	while (first->next)
	{
		ABlock* temp = first;
		first = first->next;
		temp->Destroy();
	}
	first->Destroy();
}

void Cave::ResetCave()
{
	DestroyCave();
	GenerateStart();
}

void Cave::SetCaveVisible(bool _visible)
{
	if (!first) return;

	for (int i = 0; i < 4; i++)
	{
		repeatableFloors[i]->SetActorHiddenInGame(!_visible);
	}

	ABlock* nextToHide = first;

	while (nextToHide->next)
	{
		nextToHide->SetActorHiddenInGame(!_visible);
		nextToHide = nextToHide->next;
	}
	nextToHide->SetActorHiddenInGame(!_visible);

	dwarfPawn->SetActorHiddenInGame(!_visible);

	caveVisible = _visible;
}

ABlock* Cave::GenerateBlock(FVector _pos)
{
	ABlock* block = dwarfPawn->GetWorld()->SpawnActor<ABlock>(BP_BlockClass, _pos, FRotator(), FActorSpawnParameters());
	if (!block)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Error creating block")));
		return nullptr;
	}

	block->mesh = block->GetComponentByClass<UStaticMeshComponent>();
	block->SetActorHiddenInGame(!caveVisible);

	SetBlockDataByType(block, PickRandomType());

	block->Data.health *= diffBlockHealthMult * distanceHealthMult;

	block->Data.index = blocksGenerated;
	blocksGenerated++;
	while (CheckRank());

	return block;
}

void Cave::BreakFirst()
{
	ABlock* broken = first;
	first = first->next;
	dwarfPawn->targetMetersWalked = first->xPos - 1;

	blocksBroken += broken->Data.value;

	BlockBreakDelegate.Execute(broken->Data);
	broken->Destroy();
	
	GenerateTail();
}

void Cave::Break(int _index)
{
	if (_index == 0)
	{
		BreakFirst();
		return;
	}

	ABlock* previous = first;
	for (int i = 0; i < _index - 1; i++)
	{
		previous = previous->next;
	}

	ABlock* broken = previous->next;
	previous->next = broken->next;

	blocksBroken += broken->Data.value;

	BlockBreakDelegate.Execute(broken->Data);
	broken->Destroy();
}

bool Cave::CheckRank()
{
	switch (caveRank)
	{
	case 0:
	{
		if (blocksGenerated >= 10 * BLOCK_COUNT_Y)
		{
			resourceActive[COAL_BLOCK] = true;
			resourceActive[COPPER_BLOCK] = true;
			caveRank++;
			return true;
		}
	}
	case 1:
	{
		if (blocksGenerated >= 25 * BLOCK_COUNT_Y)
		{
			resourceActive[SULFUR_BLOCK] = true;
			caveRank++;
			return true;
		}
	}
	case 2:
	{
		if (blocksGenerated >= 50 * BLOCK_COUNT_Y)
		{
			resourceActive[TIN_BLOCK] = true;
			weight[MUDROCK_BLOCK] = 750;
			caveRank++;
			return true;
		}
	}
	case 3:
	{
		if (blocksGenerated >= 100 * BLOCK_COUNT_Y)
		{
			resourceActive[IRON_BLOCK] = true;
			weight[MUDROCK_BLOCK] = 500;
			caveRank++;
			return true;
		}
	}
	}
	return false;
}

bool Cave::DamageFirst(int _damage)
{
	first->Data.health -= _damage;
	if (first->Data.health <= 0)
	{
		BreakFirst();
		return true;
	}

	return false;
}

bool Cave::Damage(int _damage, int _index)
{
	ABlock* target = first;
	first->Data.health -= _damage;
	if (first->Data.health <= 0)
	{
		Break(_index);
		return true;
	}

	return false;
}

void Cave::GenerateTail()
{
	if (lastGridPos[1] >= BLOCK_COUNT_Y - 1)
	{
		lastGridPos[0]++;
		lastGridPos[1] = 0;
	}

	else
	{
		lastGridPos[1]++;
		if (lastGridPos[1] == BLOCK_COUNT_Y - 1)
		{
			columnsBroken++;
			CheckMoveFloor();
		}
	}

	distanceHealthMult = 1 + 0.005f * blocksGenerated;

	last->next = GenerateBlock(FVector(0, (lastGridPos[0]) * BLOCK_SIZE, (BLOCK_COUNT_Y - 1 - lastGridPos[1]) * BLOCK_SIZE + BLOCK_OFFSET_Y));
	last = last->next;
	last->xPos = lastGridPos[0];
}

void Cave::SetBlockDataByType(ABlock* _block, BlockType _type)
{
	_block->Data.type = _type;
	switch (_type)
	{
	case MUDROCK_BLOCK:
		_block->Data.health = 5;
		_block->Data.yield.Push({ MUDROCK, uint64(1)});
		_block->mesh->SetMaterial(0, MudrockMat);
		break;
	case COAL_BLOCK:
		_block->Data.health = 10;
		_block->Data.yield.Push({ COAL, uint64(1) });
		_block->mesh->SetMaterial(0, CoalMat);
		break;
	case COPPER_BLOCK:
		_block->Data.health = 35;
		_block->Data.yield.Push({ COPPER, uint64(1) });
		_block->mesh->SetMaterial(0, CopperMat);
		break;
	case TIN_BLOCK:
		_block->Data.health = 35;
		_block->Data.yield.Push({ TIN, uint64(1) });
		_block->mesh->SetMaterial(0, TinMat);
		break;
	case IRON_BLOCK:
		_block->Data.health = 75;
		_block->Data.yield.Push({ IRON, uint64(1) });
		_block->mesh->SetMaterial(0, IronMat);
		break;
	case SULFUR_BLOCK:
		_block->Data.health = 20;
		_block->Data.yield.Push({ SULFUR, uint64(1) });
		_block->mesh->SetMaterial(0, SulfurMat);
		break;
	case SILVER_BLOCK:
		_block->Data.health = 100;
		_block->Data.yield.Push({ SILVER, uint64(1) });
		_block->mesh->SetMaterial(0, SilverMat);
		break;
	case OBSIDIAN_BLOCK:
		_block->Data.health = 50;
		_block->Data.yield.Push({ OBSIDIAN, uint64(1) });
		_block->mesh->SetMaterial(0, ObsidianMat);
		break;
	case PLATINUM_BLOCK:
		_block->Data.health = 250;
		_block->Data.yield.Push({ PLATINUM, uint64(1) });
		_block->mesh->SetMaterial(0, PlatinumMat);
		break;
	case DIAMOND_BLOCK:
		_block->Data.health = 1000;
		_block->Data.yield.Push({ DIAMOND, uint64(1) });
		_block->mesh->SetMaterial(0, DiamondMat);
		break;
	}
}

void Cave::CheckMoveFloor()
{
	for (int i = 0; i < 4; i++)
	{
		if (repeatableFloors[i]->GetActorLocation().Y + FLOOR_LENGTH <= dwarfPawn->GetActorLocation().Y)
		{
			repeatableFloors[i]->SetActorLocation(repeatableFloors[i]->GetActorLocation() + FVector(0, 2 * FLOOR_LENGTH, 0));
		}
	}
}

BlockType Cave::PickRandomType()
{
	// Cache this and update when dirty
	int sum = 0;
	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		if (resourceActive[i] == false) continue;
		sum += weight[i];
	}
	int select = rand() % sum;

	sum = 0;
	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		if (resourceActive[i] == false) continue;
		sum += weight[i];
		if (select < sum)
		{
			return (BlockType)i;
		}
	}

	return BlockType();
}

Cave::~Cave()
{
}

Cave::Cave()
{
	BP_BlockClass = StaticLoadClass(ABlock::StaticClass(), nullptr, TEXT("/Game/Blueprints/BP_Block.BP_Block_C"));
	FloorClass = StaticLoadClass(AStaticMeshActor::StaticClass(), nullptr, TEXT("/Game/Blueprints/FloorRoof.FloorRoof_C"));

	MudrockMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));
	CoalMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Coal.Coal"));
	CopperMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Copper.Copper"));
	TinMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Tin.Tin"));
	IronMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Iron.Iron"));
	SilverMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Silver.Silver"));
	SulfurMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Sulfur.Sulfur"));
	ObsidianMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Obsidian.Obsidian"));
	PlatinumMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Platinum.Platinum"));
	DiamondMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Diamond.Diamond"));
	MagicMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Magic.Magic"));

	weight[MUDROCK_BLOCK] = 1000;	resourceActive[MUDROCK_BLOCK] = true;
	weight[COAL_BLOCK] = 200;		resourceActive[COAL_BLOCK] = false;
	weight[COPPER_BLOCK] = 200;		resourceActive[COPPER_BLOCK] = false;
	weight[TIN_BLOCK] = 100;		resourceActive[TIN_BLOCK] = false;
	weight[IRON_BLOCK] = 50;		resourceActive[IRON_BLOCK] = false;
	weight[SILVER_BLOCK] = 50;		resourceActive[SILVER_BLOCK] = false;
	weight[SULFUR_BLOCK] = 200;		resourceActive[SULFUR_BLOCK] = false;
	weight[OBSIDIAN_BLOCK] = 20;	resourceActive[OBSIDIAN_BLOCK] = false;
	weight[PLATINUM_BLOCK] = 10;	resourceActive[PLATINUM_BLOCK] = false;
	weight[DIAMOND_BLOCK] = 5;		resourceActive[DIAMOND_BLOCK] = false;
}
