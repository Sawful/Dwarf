// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCard.h"

void UItemCard::SetItemInfo(RogueItem* _item)
{
	Name->SetText(FText::FromString(_item->name + " (Lvl: " + FString::FromInt(_item->level) + ")"));
	Name->SetColorAndOpacity(GetRarityColor(_item->rarity));
	Description->SetText(FText::FromString(_item->GetDescriptionText(_item->level + 1)));
	ItemIcon->SetBrushFromTexture(_item->icon);
}

void UItemCard::SetRelicInfo(IdleRelic* _item)
{
	Name->SetText(FText::FromString(_item->name + "\n" + _item->GetRankText()));
	Description->SetText(FText::FromString(_item->GetDescriptionText(_item->rank)));
	ItemIcon->SetBrushFromTexture(_item->icon);
}
