// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceEntryWidget.h"
#include "Components/RichTextBlock.h"

void UResourceEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UResourceEntryData* Data = Cast<UResourceEntryData>(ListItemObject);
	if (!Data) return;

	CurrentData = Data;

	// When Broadcast -> Call OnDataChanged()
	CurrentData->OnResourceChanged.AddUObject(this, &UResourceEntryWidget::OnDataChanged);
	CurrentData->ShowDelegate.BindUObject(this, &UResourceEntryWidget::Show);

	SetVisibility(CurrentData->defaultVisibility);

	UpdateText();
}

void UResourceEntryWidget::OnDataChanged()
{
	Show(true);
	UpdateText();
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, "WIDGET SHOWN");
}

void UResourceEntryWidget::Show(bool _show)
{
	if (_show)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UResourceEntryWidget::UpdateText()
{
	if (CurrentData == nullptr) return;
	ResourceText->SetText(FText::FromString(CurrentData->ResourceIcon + ": " + CurrentData->Amount.ToStringTrunc()));
}