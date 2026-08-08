// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCard.h"

void UItemCard::SetItemInfo(RogueItem* _item)
{
	Name->SetText(FText::FromString(_item->name));
	Description->SetText(FText::FromString(_item->description));
	ItemIcon->SetBrushFromTexture(_item->icon);
}
