// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/Widget.h"
#include "Components/EditableText.h"
#include "DwarfHUD.generated.h"

/**
 * 
 */
UCLASS()
class DWARF_API ADwarfHUD : public AHUD
{
	GENERATED_BODY()

	void BeginPlay();
};
