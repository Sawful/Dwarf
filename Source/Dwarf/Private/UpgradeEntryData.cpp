// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeEntryData.h"
#include "Block.h"

void UUpgradeEntryData::SetLevel(int _level)
{
	level = _level;
	OnUpgradeChanged.Broadcast();
}

void UUpgradeEntryData::SetCostText(FString _costText)
{
	costText = _costText;
	OnUpgradeChanged.Broadcast();
}

void UUpgradeEntryData::SetDamageText(FString _damageText)
{
	damageText = _damageText;
	OnUpgradeChanged.Broadcast();
}
