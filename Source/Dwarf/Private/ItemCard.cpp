// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCard.h"

void UItemCard::SetItemInfo(URogueItem* _item)
{
	Name->SetText(FText::FromString(_item->name + " (Lvl: " + FString::FromInt(_item->level) + ")"));
	Name->SetColorAndOpacity(GetRarityColor(_item->rarity));
	Description->SetText(FText::FromString(_item->GetDescriptionText(_item->level + 1)));
	UTexture2D* tex = _item->icon;
	if (!IsValid(tex))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Icon invalid");
		return;
	}
	ItemIcon->SetBrushFromTexture(tex);
}

void UItemCard::SetRelicInfo(IdleRelic* _item)
{
	Name->SetText(FText::FromString(_item->name + "\n" + _item->GetRankText()));
	if(_item->count + 1 >= _item->countRequired) Description->SetText(FText::FromString(_item->GetDescriptionText(_item->rank + 1)));
	else Description->SetText(FText::FromString(_item->GetDescriptionText(_item->rank)));

	UTexture2D* tex = _item->icon;
	if (!IsValid(tex))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Icon invalid");
		return;
	}
	ItemIcon->SetBrushFromTexture(tex);
}
