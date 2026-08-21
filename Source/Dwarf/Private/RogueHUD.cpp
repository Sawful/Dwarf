// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueHUD.h"
#include "RogueItem.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"

void URogueHUD::SetLevel(int _level, float _progress)
{
	LevelText->SetText(FText::FromString("Rogue Level: " + FString::FromInt(_level)));
	LevelProgress->SetCompletion(_progress);
}

void URogueHUD::SetDistanceText(int _distance)
{
	DistanceTraveledText->SetText(FText::FromString("Distance Traveled: " + FString::FromInt(_distance) + "m"));
}

void URogueHUD::SetPressure(BigNumber _current, BigNumber _max)
{
	PressureBar->SetPercent((float)(_current / _max));
	// Change color?
	// Text?
}

UUpgradeEntryWidget* URogueHUD::AddItemWidget(URogueItem* _item)
{
	UUpgradeEntryWidget* widget = CreateWidget<UUpgradeEntryWidget, UWrapBox*>(ItemBox, ItemBoxClass);
	widget->level = 1;
	widget->name = _item->name;
	widget->SetPadding(16);
	widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
	widget->descriptionText = _item->GetDescriptionText(_item->level);
	if (!IsValid(_item->icon))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Icon invalid");
	}
	widget->Icon->SetBrushFromTexture(_item->icon);
	ItemBox->AddChild(widget);

	return widget;
}
