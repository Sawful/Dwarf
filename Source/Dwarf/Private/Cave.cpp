// Fill out your copyright notice in the Description page of Project Settings.


#include "Cave.h"
#include "DwarfPlayerState.h"

void Cave::GenerateStart()
{
	if (BP_BlockClass == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Could not create block class.")));
		return;
	}

	first = GenerateBlock(FVector(0, 1 * BLOCK_SIZE, (BLOCK_COUNT_Y - 1) * BLOCK_SIZE + BLOCK_OFFSET_Y));
	last = first;
	int j = 1;

	for (int i = 0; i < BLOCK_COUNT_X; i++)
	{
		for (; j < BLOCK_COUNT_Y; j++)
		{
			last->next = GenerateBlock(FVector(0, (i + 1) * BLOCK_SIZE, (BLOCK_COUNT_Y - 1 - j) * BLOCK_SIZE + BLOCK_OFFSET_Y));
			last = last->next;
		}
		j = 0;
	}
	lastGridPos[0] = BLOCK_COUNT_X;
	lastGridPos[1] = BLOCK_COUNT_Y;
}

void Cave::ResetCave()
{
	// Destroy every block
	while (first->next)
	{
		ABlock* temp = first;
		first = first->next;
		temp->Destroy();
	}
	first->Destroy();

	GenerateStart();
}

void Cave::SetCaveVisible(bool _visible)
{
	while (first->next)
	{
		first->SetActorHiddenInGame(!_visible);
		first = first->next;
	}
	first->SetActorHiddenInGame(!_visible);
	caveVisible = _visible;
}

ABlock* Cave::GenerateBlock(FVector _pos)
{
	FActorSpawnParameters SpawnParam = FActorSpawnParameters();
	FRotator rot = FRotator();
	ABlock* block = player->GetWorld()->SpawnActor<ABlock>(BP_BlockClass, _pos, rot, SpawnParam);
	if (!block)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Error creating block")));
		return nullptr;
	}

	block->mesh = block->GetComponentByClass<UStaticMeshComponent>();
	block->SetActorHiddenInGame(!caveVisible);

	block->Data.type = BlockType(rand() % 3);

	switch (block->Data.type)
	{
	case DIRT_BLOCK:
		block->Data.yield.Push({ DIRT, rand() % 3 + 2 });
		block->mesh->SetMaterial(0, DirtMat);
		break;
	case STONE_BLOCK:
		block->Data.yield.Push({ STONE, rand() % 2 + 1 });
		block->mesh->SetMaterial(0, StoneMat);
		break;
	case ORE_BLOCK:
		block->Data.yield.Push({ STONE, rand() % 2 + 1 });
		block->Data.yield.Push({ ORE, rand() % 2 + 1 });
		block->mesh->SetMaterial(0, OreMat);
		break;
	}
	return block;
}

void Cave::BreakFirst()
{
	ABlock* broken = first;
	first = first->next;
	broken->Destroy();
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
			player->MoveForward();
		}
	}

	last->next = GenerateBlock(FVector(0, (lastGridPos[0]) * BLOCK_SIZE, (BLOCK_COUNT_Y - 1 - lastGridPos[1]) * BLOCK_SIZE + BLOCK_OFFSET_Y));
	last = last->next;
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

Cave::Cave()
{
	BP_BlockClass = StaticLoadClass(ABlock::StaticClass(), nullptr, TEXT("/Game/BP_Block.BP_Block_C"));
	
	DirtMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Dirt.Dirt"));
	StoneMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Stone.Stone"));
	OreMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/Ore.Ore"));
}
