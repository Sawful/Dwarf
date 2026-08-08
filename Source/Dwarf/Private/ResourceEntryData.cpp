// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceEntryData.h"

void UResourceEntryData::SetAmount(BigNumber _NewAmount)
{
    Amount = _NewAmount;
    AmountString = Amount.ToString();
    OnResourceChanged.Broadcast();
}

void UResourceEntryData::AddAmount(BigNumber _Add)
{
    Amount += _Add;
    AmountString = Amount.ToString();
    OnResourceChanged.Broadcast();
}
