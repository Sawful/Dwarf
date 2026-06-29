// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UUpgradeEntryWidget::SetCost(TArray<ResourceData> cost)
{
	FString string = "Cost: \n";
	for(auto resource : cost)
	{
		string += GetResourceName(resource.Type);
		string += ": ";
		string += FString::FromInt(resource.Amount);
		string += "\n";
	}

	Cost->SetText(FText::FromString(string));
}

void UUpgradeEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	CurrentData = Cast<UUpgradeEntryData>(ListItemObject);
	if (CurrentData == nullptr) return;

	Name->SetText(FText::FromString(CurrentData->upgradeName));
	Button->OnClicked.Add(CurrentData->upgradeDelegate);
}
