// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceData.h"

enum UpgradeType
{
	STRONG_ARMS = 0,
	DRILL,
	BOOM,

	UPGRADE_COUNT
};

DECLARE_DELEGATE_RetVal_OneParam(TArray<ResourceData>, FOnGetCost, int)
struct ResourceUpgrade
{
	UPROPERTY()
	FString upgradeFunctionName;

	UPROPERTY()
	FString displayName;

	UPROPERTY()
	int upgradeLevel;

	UPROPERTY()
	TArray<ResourceData> costCached;

	FOnGetCost costDelegate;

	TArray<ResourceData> GetCost(int _count);

};