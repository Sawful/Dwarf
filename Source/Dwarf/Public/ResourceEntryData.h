// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BigNumber.h"
#include "ResourceEntryData.generated.h"
DECLARE_MULTICAST_DELEGATE(FOnResourceChanged);

UCLASS()
class DWARF_API UResourceEntryData : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FString ResourceIcon;

    UPROPERTY(BlueprintReadWrite)
    FString AmountString;
    BigNumber Amount;

    FOnResourceChanged OnResourceChanged;

    void SetAmount(BigNumber _NewAmount);
    void AddAmount(BigNumber _Add);
};
