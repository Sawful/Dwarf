// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"

enum DamageTextType
{
	NORMAL,
	AUTO,
	CRITICAL
};

struct DWARF_API DamageSource
{
	float BlockDamageMultiplier[BLOCK_COUNT];
	DamageTextType TextType;
	DamageSource() { std::fill_n(BlockDamageMultiplier, BLOCK_COUNT, 1.0f); };
};