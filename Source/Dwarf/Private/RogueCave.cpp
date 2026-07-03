// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCave.h"
#include "DwarfPlayerState.h"

ABlock* RogueCave::GenerateBlock(FVector _pos)
{
	// TODO: Find every event and thing possible in a rogue run

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

