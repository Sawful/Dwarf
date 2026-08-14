// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"


void UUpgradeEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	CurrentData = Cast<UUpgradeEntryData>(ListItemObject);
	if (CurrentData == nullptr) return;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(CurrentData->level));

	//Name->SetText(FText::FromString(CurrentData->upgradeName));
	Button->OnClicked.Add(CurrentData->upgradeDelegate);

	//CurrentData->OnUpgradeChanged.AddUObject(this, &UUpgradeEntryWidget::UpdateText);
	//UpdateText();
}
