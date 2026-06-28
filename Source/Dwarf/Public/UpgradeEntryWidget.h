// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UpgradeEntryData.h"
#include "UpgradeEntryWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class DWARF_API UUpgradeEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	UUpgradeEntryData* CurrentData;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* Name;


protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
