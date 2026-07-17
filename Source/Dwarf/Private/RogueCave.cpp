// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCave.h"
#include "DwarfPlayerState.h"

ABlock* RogueCave::GenerateBlock(FVector _pos)
{
	// TODO: Find every event and thing possible in a rogue run

	ABlock* block = dwarfPawn->GetWorld()->SpawnActor<ABlock>(BP_BlockClass, _pos, FRotator(), FActorSpawnParameters());
	if (!block)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Error creating block")));
		return nullptr;
	}

	block->mesh = block->GetComponentByClass<UStaticMeshComponent>();
	block->SetActorHiddenInGame(!caveVisible);

	SetBlockDataByType(block, BlockType(rand() % 3));

	block->Data.health *= (block->Data.health + blocksGenerated) / block->Data.health;
	block->Data.health *= powf(1.02f, powf(blocksGenerated, 0.5f));
	blocksGenerated++;

	return block;
}

