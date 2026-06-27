// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DwarfGameState.generated.h"

UCLASS()
class DWARF_API ADwarfGameState : public AGameStateBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
};
