// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
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
    int32 Amount;

    FOnResourceChanged OnResourceChanged;

    void SetAmount(int32 _NewAmount);
    void AddAmount(int32 _Add);
};
