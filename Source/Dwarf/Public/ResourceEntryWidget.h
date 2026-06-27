// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ResourceEntryData.h"
#include "ResourceEntryWidget.generated.h"

class UEditableText;

UCLASS()
class DWARF_API UResourceEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* ResourceText;

	UPROPERTY()
	UResourceEntryData* CurrentData;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void OnDataChanged();

	UFUNCTION()
	void UpdateText();

};
