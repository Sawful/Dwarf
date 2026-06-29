// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum ResourceType {
	DIRT = 0,
	STONE = 1,
	ORE = 2,

	RESOURCE_COUNT
};

struct DWARF_API ResourceData
{
	ResourceType Type;
	int Amount;
};
