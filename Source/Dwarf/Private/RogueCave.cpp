// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCave.h"
#include "DwarfPlayerState.h"

void RogueCave::BreakFirst()
{
	ABlock* broken = first;
	first = first->next;

	blocksBroken += broken->Data.value;
	BlockBreakDelegate.Execute(broken->Data);

	broken->Destroy();
	dwarfPawn->targetMetersWalked = first->pos[0] - 1;
	GenerateTail();

}

ABlock* RogueCave::GenerateBlock(FVector _pos)
{
	ABlock* block = dwarfPawn->GetWorld()->SpawnActor<ABlock>(BP_BlockClass, _pos, FRotator(), FActorSpawnParameters());
	if (!block)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Error creating block")));
		return nullptr;
	}

	block->mesh = block->GetComponentByClass<UStaticMeshComponent>();
	block->SetActorHiddenInGame(!caveVisible);

	SetBlockDataByType(block, BlockType(rand() % 3));

	diffBlockHealthMult = powf(1.02f, powf(blocksGenerated, 0.5f));

	block->Data.health *= diffBlockHealthMult;
	block->Data.health *= (block->Data.health + blocksGenerated) / block->Data.health;

	cavePressure = 1.0f + blocksGenerated * 0.02f;
	block->Data.pressureValue = cavePressure;
	block->Data.pressureRegen = cavePressure;
	blocksGenerated++;

	return block;
}

void RogueCave::GenerateTail()
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

	if (lastGridPos[0] % 100 == 0)
	{
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector pos = FVector(0, (lastGridPos[0]) * BLOCK_SIZE + 1.5f * BLOCK_SIZE, (BLOCK_COUNT_Y * BLOCK_SIZE) / 2.0f);
		ABlock* block = dwarfPawn->GetWorld()->SpawnActor<ABlock>(BP_BlockClass, pos, FRotator(), param);
		block->mesh = block->GetComponentByClass<UStaticMeshComponent>();
		block->SetActorHiddenInGame(!caveVisible);
		block->SetActorScale3D(FVector(1, BLOCK_COUNT_Y, BLOCK_COUNT_Y));

		last->next = block;
		last = last->next;
		last->pos[0] = lastGridPos[0];
		last->pos[1] = lastGridPos[1];

		lastGridPos[0] += 3;
		lastGridPos[1] = BLOCK_COUNT_Y - 1;

		block->Data.health = 1000;
		block->Data.health *= diffBlockHealthMult;
		block->Data.expValue = lastGridPos[0] * 4;
		block->mesh->SetMaterial(0, MagicMat);

		cavePressure = 1.0f + blocksGenerated * 0.02f;
		block->Data.pressureValue = cavePressure * 8;
		block->Data.pressureRegen = cavePressure * 8;
		block->Data.value = BLOCK_COUNT_Y * BLOCK_COUNT_Y;
		blocksGenerated += BLOCK_COUNT_Y * BLOCK_COUNT_Y;
		return;
	}
	else if (lastGridPos[0] % 25 == 0)
	{
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector pos = FVector(0, (lastGridPos[0]) * BLOCK_SIZE, (BLOCK_COUNT_Y * BLOCK_SIZE) / 2.0f);
		ABlock* block = dwarfPawn->GetWorld()->SpawnActor<ABlock>(BP_BlockClass, pos, FRotator(), param);
		block->mesh = block->GetComponentByClass<UStaticMeshComponent>();
		block->SetActorHiddenInGame(!caveVisible);
		block->SetActorScale3D(FVector(1, 1, BLOCK_COUNT_Y));

		lastGridPos[1] = BLOCK_COUNT_Y - 1;

		block->Data.health = 200;
		block->Data.health *= diffBlockHealthMult;
		block->Data.expValue = lastGridPos[0];
		block->mesh->SetMaterial(0, MagicMat);

		last->next = block;
		last = last->next;
		last->pos[0] = lastGridPos[0];
		last->pos[1] = lastGridPos[1];

		cavePressure = 1.0f + blocksGenerated * 0.02f;
		block->Data.pressureValue = cavePressure * 2;
		block->Data.pressureRegen = cavePressure * 2;
		block->Data.value = BLOCK_COUNT_Y;
		blocksGenerated += BLOCK_COUNT_Y;

		return;
	}

	last->next = GenerateBlock(FVector(0, (lastGridPos[0]) * BLOCK_SIZE, (BLOCK_COUNT_Y - 1 - lastGridPos[1]) * BLOCK_SIZE + BLOCK_OFFSET_Y));
	last = last->next;
	last->pos[0] = lastGridPos[0];
	last->pos[1] = lastGridPos[1];
}