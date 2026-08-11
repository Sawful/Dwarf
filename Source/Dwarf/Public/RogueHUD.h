// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/ProgressBar.h"
#include "ExpBarWidget.h"
#include "RogueHUD.generated.h"

UCLASS()
class DWARF_API URogueHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* MenuButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* LevelText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UExpBarWidget* LevelProgress;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* ItemBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* PressureBar;
};
