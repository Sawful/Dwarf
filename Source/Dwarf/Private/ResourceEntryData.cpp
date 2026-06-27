// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceEntryData.h"

void UResourceEntryData::SetAmount(int32 _NewAmount)
{
    Amount = _NewAmount;
    OnResourceChanged.Broadcast();
}

void UResourceEntryData::AddAmount(int32 _Add)
{
    Amount += _Add;
    OnResourceChanged.Broadcast();
}
