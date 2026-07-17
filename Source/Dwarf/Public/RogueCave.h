// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cave.h"

class DWARF_API RogueCave: public Cave
{
	int blocksGenerated = 0;
public:
	virtual ABlock* GenerateBlock(FVector _pos) override;

};
