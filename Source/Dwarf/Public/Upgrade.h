// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceData.h"
#include "UpgradeEntryWidget.h"

enum UpgradeType
{
	STRONG_ARMS = 0,	// Click damage
	DRILL,				// Auto dmg
	BOOM,				// Auto dmg column
	EARTHQUAKE,			// Auto dmg area
	LASER,				// Auto dmg row
	PRECISION,			// Click crit chance
	GOLD_LOVER,			// Global Yield

	UPGRADE_COUNT
};

DECLARE_DELEGATE_RetVal_OneParam(TArray<ResourceData>, FOnGetCost, unsigned int)
struct ResourceUpgrade
{
	UPROPERTY()
	FString upgradeFunctionName;

	UPROPERTY()
	FString displayName;

	UPROPERTY()
	FString description;

	UPROPERTY()
	unsigned int upgradeLevel;

	UPROPERTY()
	TArray<ResourceData> costCached;

	// Texture/Image
	UTexture* icon;

	UUpgradeEntryWidget* widget;

	FOnGetCost costDelegate;

	TArray<ResourceData> GetCostAndCache(int _count);
	void UpdateTooltipText();

};