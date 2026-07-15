// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeEntryData.h"
#include "Block.h"

void UUpgradeEntryData::SetLevel(int _level)
{
	level = _level;
	OnUpgradeChanged.Broadcast();
}

void UUpgradeEntryData::SetCostText(TArray<ResourceData> cost)
{
	FString string = "Cost: \n";
	for (auto resource : cost)
	{
		string += GetResourceName(resource.Type);
		string += ": ";
		string += FString::FromInt(resource.Amount);
		string += "\n";
	}

	costText = string;
	OnUpgradeChanged.Broadcast();
}

void UUpgradeEntryData::SetDamageText(FString _damageText)
{
	damageText = _damageText;
	OnUpgradeChanged.Broadcast();
}
