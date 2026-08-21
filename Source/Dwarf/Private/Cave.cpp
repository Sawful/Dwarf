// Fill out your copyright notice in the Description page of Project Settings.


#include "Cave.h"
#include "DwarfPawn.h"
#include "Engine/StaticMeshActor.h"
#include "Components/TextRenderComponent.h"

#define AREA_DEPTH 3
#define ROW_DEPTH 5
#define ROW_HEIGHT BLOCK_COUNT_Y - 1

void Cave::GenerateStart()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//GEngine->AddOnScreenDebugMessage(0, 10, FColor::Emerald, FString::FromInt(IsValid(FloorClass)));

	repeatableFloors[0] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, FLOOR_LENGTH, 800), FRotator(), param);
	repeatableFloors[1] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, 0, 800), FRotator(), param);
	repeatableFloors[2] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, FLOOR_LENGTH, -400), FRotator(), param);
	repeatableFloors[3] = dwarfPawn->GetWorld()->SpawnActor<AStaticMeshActor>(FloorClass, FVector(0, 0, -400), FRotator(), param);

	diffBlockHealthMult = powf(blockHealthDiffScaling, difficultyLevel);
	yieldMultiplier = powf(blockRewardsDiffScaling, difficultyLevel);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Tried generating cave")));
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
	blocksGenerated = 0;
	blocksBroken = 0;

	weight[MUDROCK_BLOCK] = 1000;
	weight[COAL_BLOCK] = 0;
	weight[COPPER_BLOCK] = 0;
	weight[TIN_BLOCK] = 0;
	weight[IRON_BLOCK] = 0;
	weight[SILVER_BLOCK] = 0;
	weight[SULFUR_BLOCK] = 0;
	weight[OBSIDIAN_BLOCK] = 0;
	weight[PLATINUM_BLOCK] = 0;
	weight[DIAMOND_BLOCK] = 0;
	caveRank = 0;

	distanceHealthMult = 1.0f;

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

void Cave::CreateDamageText(BigNumber _damage, DamageTextType _type, FVector _position)
{
	if (!DamageTextClass) return;
	if (!caveVisible) return;

	// Block offset
	_position.X += -100;
	_position.Y += ((float)((rand() % 100) - 50));
	_position.Z += ((float)((rand() % 100) - 50));

	FRotator rotator = FRotator(0, 180, 0);
	ATextRenderActor* damageText = dwarfPawn->GetWorld()->SpawnActor<ATextRenderActor>(DamageTextClass, _position, rotator, FActorSpawnParameters());
	UTextRenderComponent* textRender = damageText->GetTextRender();
	switch (_type)
	{
	case NORMAL:
	{
		textRender->SetTextRenderColor(FColor(0xFFFF0000));
		textRender->WorldSize = 40;
		break;
	}
	case AUTO:
	{
		textRender->SetTextRenderColor(FColor(0xFF2F2F2F));
		textRender->WorldSize = 35;
		break;
	}
	case CRITICAL:
	{
		textRender->SetTextRenderColor(FColor(0xFFF0F000));
		textRender->WorldSize = 50;
		break;
	}

	}

	textRender->SetText(FText::FromString(_damage.ToStringTrunc()));
}

void Cave::BreakFirst()
{
	ABlock* broken = first;
	first->DisconnectFromList();
	first = first->next;
	dwarfPawn->targetMetersWalked = first->pos[0] - 1;

	blocksBroken += broken->Data.value;

	BlockBreakDelegate.Execute(broken->Data);
	broken->Destroy();
	
	GenerateTail();
}

void Cave::Break(ABlock* _broken)
{
	if (_broken == first)
	{
		first = first->next;
		dwarfPawn->targetMetersWalked = first->pos[0] - 1;
	}
	if (_broken == last)
	{
		last = last->previous;
	}

	blocksBroken += _broken->Data.value;
	BlockBreakDelegate.Execute(_broken->Data);
	_broken->DisconnectFromList();
	_broken->Destroy();
	GenerateTail();
}

bool Cave::CheckRank()
{
	switch (caveRank)
	{
	case 0:
	{
		if (blocksGenerated >= 10 * BLOCK_COUNT_Y)
		{
			weight[COAL_BLOCK] = 200;
			weight[COPPER_BLOCK] = 200;

			caveRank++;
			return true;
		}
	}
	case 1:
	{
		if (blocksGenerated >= 25 * BLOCK_COUNT_Y)
		{
			weight[SULFUR_BLOCK] = 200;

			caveRank++;
			return true;
		}
	}
	case 2:
	{
		if (blocksGenerated >= 50 * BLOCK_COUNT_Y)
		{
			weight[TIN_BLOCK] = 100;
			weight[MUDROCK_BLOCK] = 750;

			caveRank++;
			return true;
		}
	}
	case 3:
	{
		if (blocksGenerated >= 100 * BLOCK_COUNT_Y)
		{
			weight[IRON_BLOCK] = 50;
			weight[MUDROCK_BLOCK] = 500;

			caveRank++;
			return true;
		}
		
	}
	case 4:
	{
		if (blocksGenerated >= 150 * BLOCK_COUNT_Y)
		{
			weight[MUDROCK_BLOCK] = 400;
			weight[SILVER_BLOCK] = 50;

			caveRank++;
			return true;
		}
	}
	case 5:
	{
		if (blocksGenerated >= 250 * BLOCK_COUNT_Y)
		{
			weight[MUDROCK_BLOCK] = 250;
			weight[OBSIDIAN_BLOCK] = 20;
			weight[PLATINUM_BLOCK] = 10;
			weight[DIAMOND_BLOCK] = 5;

			caveRank++;
			return true;
		}
	}
	case 6:
	{
		if (blocksGenerated >= 500 * BLOCK_COUNT_Y)
		{
			weight[MUDROCK_BLOCK] = 200;
			weight[TIN_BLOCK] = 150;
			weight[IRON_BLOCK] = 100;
			weight[SILVER_BLOCK] = 100;

			caveRank++;
			return true;
		}
	}
	case 7:
	{
		if (blocksGenerated >= 1000 * BLOCK_COUNT_Y)
		{
			weight[MUDROCK_BLOCK] = 150;
			weight[COAL_BLOCK] = 150;
			weight[COPPER_BLOCK] = 150;

			weight[OBSIDIAN_BLOCK] = 50;
			weight[PLATINUM_BLOCK] = 25;
			weight[DIAMOND_BLOCK] = 10;

			caveRank++;
			return true;
		}
	}
	}
	return false;
}

void Cave::DamageFirst(BigNumber _damage, DamageSource _source)
{
	CreateDamageText(_damage, _source.TextType, first->GetActorLocation());
	first->Data.health -= _damage;
	if (first->Data.health <= (BigNumber)0)
	{
		Break(first);
	}
}

void Cave::DamageFirstColumn(BigNumber _damage, DamageSource _source)
{	
	TArray<ABlock*> tempBroken;

	int column = first->pos[0];
	ABlock* target = first;
	for (int i = 0; i < BLOCK_COUNT_Y; i++)
	{
		// As soon as we change column
		if (target->pos[0] != column) break;

		CreateDamageText(_damage, _source.TextType, target->GetActorLocation());
		target->Data.health -= _damage;
		if (target->Data.health <= (BigNumber)0)
		{
			tempBroken.Add(target);
		}

		target = target->next;
	}

	// Break blocks at the end to avoid messing with the links during the initial loop.
	for (ABlock* block : tempBroken)
	{
		Break(block);
	}

}

void Cave::DamageArea(BigNumber _damage, DamageSource _source)
{
	TArray<ABlock*> tempBroken;

	int column = first->pos[0];
	ABlock* target = first;
	for (int i = 0; i < BLOCK_COUNT_Y * AREA_DEPTH; i++)
	{
		// As soon as we change column
		if (target->pos[0] >= column + AREA_DEPTH) break;

		CreateDamageText(_damage, _source.TextType, target->GetActorLocation());
		target->Data.health -= _damage;
		if (target->Data.health <= (BigNumber)0)
		{
			tempBroken.Add(target);
		}

		target = target->next;
	}

	// Break blocks at the end to avoid messing with the links during the initial loop.
	for (ABlock* block : tempBroken)
	{
		Break(block);
	}
}

void Cave::DamageRow(BigNumber _damage, DamageSource _source)
{
	TArray<ABlock*> tempBroken;

	int column = first->pos[0];
	ABlock* target = first;
	for (int i = 0; i < BLOCK_COUNT_Y * ROW_DEPTH; i++)
	{
		// As soon as we change column
		if (target->pos[0] >= column + ROW_DEPTH) break;
		if (target->pos[1] != ROW_HEIGHT)
		{
			target = target->next;
			continue;
		}

		CreateDamageText(_damage, _source.TextType, target->GetActorLocation());
		target->Data.health -= _damage;
		if (target->Data.health <= (BigNumber)0)
		{
			Break(target);
		}

		break;
	}

	// Break blocks at the end to avoid messing with the links during the initial loop.
	for (ABlock* block : tempBroken)
	{
		Break(block);
	}
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
	last->next->previous = last;
	last = last->next;
	last->pos[0] = lastGridPos[0];
	last->pos[1] = lastGridPos[1];
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
		//if (resourceActive[i] == false) continue;
		sum += weight[i];
	}
	int select = rand() % sum;

	sum = 0;
	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		//if (resourceActive[i] == false) continue;
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

	weight[MUDROCK_BLOCK] = 1000;	
	weight[COAL_BLOCK] = 0;		
	weight[COPPER_BLOCK] = 0;	
	weight[TIN_BLOCK] = 0;		
	weight[IRON_BLOCK] = 0;		
	weight[SILVER_BLOCK] = 0;	
	weight[SULFUR_BLOCK] = 0;	
	weight[OBSIDIAN_BLOCK] = 0;	
	weight[PLATINUM_BLOCK] = 0;	
	weight[DIAMOND_BLOCK] = 0;	
}
