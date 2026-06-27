// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceEntryWidget.h"
#include "Components/EditableText.h"

void UResourceEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UResourceEntryData* Data = Cast<UResourceEntryData>(ListItemObject);
	if (!Data) return;

	CurrentData = Data;

	// When Broadcast -> Call OnDataChanged()
	CurrentData->OnResourceChanged.AddUObject(this, &UResourceEntryWidget::OnDataChanged);

	UpdateText();
}

void UResourceEntryWidget::OnDataChanged()
{
	UpdateText();
}

void UResourceEntryWidget::UpdateText()
{
	if (CurrentData == nullptr) return;
	ResourceText->SetText(FText::FromString(CurrentData->ResourceName + ": " + FString::FromInt(CurrentData->Amount)));
}