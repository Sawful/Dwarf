// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "ChatUserWidget.generated.h"

UCLASS()
class DWARF_API UChatUserWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* ChatBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UScrollBox* Chat;

	UFUNCTION()
	void PushMessage(const FText& text, ETextCommit::Type InCommitType);


};
