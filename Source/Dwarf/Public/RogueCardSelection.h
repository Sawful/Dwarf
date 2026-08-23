#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemCard.h"
#include "RoguePlayerData.h"
#include "RogueCardSelection.generated.h"

UCLASS()
class DWARF_API URogueCardSelection : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetRogueData(URoguePlayerData* _rogueData);

	UPROPERTY(EditAnywhere)
	URoguePlayerData* rogueData;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UItemCard* CardLeft;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UItemCard* CardMiddle;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UItemCard* CardRight;
};
