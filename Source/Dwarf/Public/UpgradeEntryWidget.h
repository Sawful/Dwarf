// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UpgradeEntryData.h"
#include "Block.h"
#include "UpgradeEntryWidget.generated.h"

class UButton;
class UTextBlock;
class URichTextBlock;

UCLASS()
class DWARF_API UUpgradeEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	UUpgradeEntryData* CurrentData;
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	URichTextBlock* LevelText;

	UPROPERTY(BlueprintReadOnly)
	FString name;
	UPROPERTY(BlueprintReadOnly)
	int level;
	UPROPERTY(BlueprintReadOnly)
	FString costText;
	UPROPERTY(BlueprintReadOnly)
	FString damageText;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
