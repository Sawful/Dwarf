// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UUpgradeEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	CurrentData = Cast<UUpgradeEntryData>(ListItemObject);
	if (CurrentData == nullptr) return;

	Name->SetText(FText::FromString(CurrentData->upgradeName));
	Button->OnClicked.Add(CurrentData->upgradeDelegate);
}
