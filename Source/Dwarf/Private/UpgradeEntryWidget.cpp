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

void UUpgradeEntryWidget::UpdateText()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Updating");
	if (CurrentData == nullptr) return;
	Level->SetText(FText::FromString("Level " + FString::FromInt(CurrentData->level)));
	Cost->SetText(FText::FromString(CurrentData->costText));
	Damage->SetText(FText::FromString(CurrentData->damageText));

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Update done");
}

void UUpgradeEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	CurrentData = Cast<UUpgradeEntryData>(ListItemObject);
	if (CurrentData == nullptr) return;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(CurrentData->level));

	Name->SetText(FText::FromString(CurrentData->upgradeName));
	Button->OnClicked.Add(CurrentData->upgradeDelegate);

	CurrentData->OnUpgradeChanged.AddUObject(this, &UUpgradeEntryWidget::UpdateText);
	UpdateText();
}
