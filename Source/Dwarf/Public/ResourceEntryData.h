// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Components/Widget.h"
#include "BigNumber.h"
#include "ResourceEntryData.generated.h"
DECLARE_MULTICAST_DELEGATE(FOnResourceChanged);
DECLARE_DELEGATE_OneParam(FShowDelegate, bool);

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
    FShowDelegate ShowDelegate;

    ESlateVisibility defaultVisibility = ESlateVisibility::Hidden;

    void SetAmount(BigNumber _NewAmount);
    void AddAmount(BigNumber _Add);
};
