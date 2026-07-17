// Fill out your copyright notice in the Description page of Project Settings.


#include "Upgrade.h"

TArray<ResourceData> ResourceUpgrade::GetCost()
{
	costCached = costDelegate.Execute(upgradeLevel);
	return costCached;
}