// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatUserWidget.h"
#include "Components/TextBlock.h"

void UChatUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	FScriptDelegate changeDelegate;
	changeDelegate.BindUFunction(this, "PushMessage");
	ChatBar->OnTextCommitted.Add(changeDelegate);
}

void UChatUserWidget::PushMessage(const FText& text, ETextCommit::Type InCommitType)
{
	if (InCommitType != ETextCommit::OnEnter) return;

	UTextBlock* newMessage = NewObject<UTextBlock>(this);
	newMessage->SetText(text);
	Chat->AddChild(newMessage);
}
