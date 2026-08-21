#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "RogueItem.h"
#include "IdleRelic.h"

#include "ItemCard.generated.h"

UCLASS()
class DWARF_API UItemCard : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* SelectButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* ItemIcon;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* Name;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* Description;

	void SetItemInfo(URogueItem* _item);
	void SetRelicInfo(IdleRelic* _item);
};
