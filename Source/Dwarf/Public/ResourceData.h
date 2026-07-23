// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum ResourceType {
	DIRT = 0,
	STONE,
	ORE,
	ORE2,
	ORE3,
	ORE4,
	ORE5,

	RESOURCE_COUNT
};

struct DWARF_API ResourceData
{
	ResourceType Type;
	int Amount;
};
