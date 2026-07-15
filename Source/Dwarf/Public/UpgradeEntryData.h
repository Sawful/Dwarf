// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ResourceData.h"
#include "UpgradeEntryData.generated.h"
DECLARE_MULTICAST_DELEGATE(FOnUpgradeChanged);

UCLASS()
class DWARF_API UUpgradeEntryData : public UObject
{
	GENERATED_BODY()
	
public:
	FScriptDelegate upgradeDelegate;

	UPROPERTY()
	FString upgradeName;

	UPROPERTY()
	int level = 0;

	UPROPERTY()
	FString damageText;
	UPROPERTY()
	FString costText;

	FOnUpgradeChanged OnUpgradeChanged;

	void SetLevel(int _level);
	void SetCostText(TArray<ResourceData> cost);
	void SetDamageText(FString _damageText);
};
