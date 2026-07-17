// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ExpBarWidget.h"
#include "AutoAttackerDisplay.generated.h"


UCLASS()
class DWARF_API UAutoAttackerDisplay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* Image;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UExpBarWidget* ProgressBar;
};
