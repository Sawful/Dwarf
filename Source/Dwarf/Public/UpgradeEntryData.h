// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UpgradeEntryData.generated.h"

UCLASS()
class DWARF_API UUpgradeEntryData : public UObject
{
	GENERATED_BODY()
	
public:
	FScriptDelegate upgradeDelegate;

	UPROPERTY()
	FString upgradeName;
};
