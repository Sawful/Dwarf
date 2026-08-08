// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigNumber.h"

enum ResourceType {
	MUDROCK = 0,
	COAL,
	COPPER,
	TIN,
	IRON,
	SULFUR,
	SILVER,
	OBSIDIAN,
	PLATINUM,
	DIAMOND,

	RESOURCE_COUNT
};

struct DWARF_API ResourceData
{
	ResourceType Type;
	BigNumber Amount;
};
