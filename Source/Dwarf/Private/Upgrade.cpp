// Fill out your copyright notice in the Description page of Project Settings.


#include "Upgrade.h"

TArray<ResourceData> ResourceUpgrade::GetCost(int _count = 1)
{
	costCached.Empty(costCached.Num());
	costCached = costDelegate.Execute(upgradeLevel);
	int typeCount = costCached.Num();
	for (int i = 1; i < _count; i++)
	{
		TArray<ResourceData> currentCost = costDelegate.Execute(upgradeLevel + i);
		for (int j = 0; j < typeCount; j++)
		{
			costCached[j].Amount += currentCost[j].Amount;
		}
	}
	
	return costCached;
}