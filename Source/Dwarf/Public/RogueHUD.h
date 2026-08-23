#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/ProgressBar.h"

#include "BigNumber.h"

#include "ExpBarWidget.h"
#include "UpgradeEntryWidget.h"
#include "RogueCardSelection.h"

#include "RogueHUD.generated.h"

class URogueItem;

UCLASS()
class DWARF_API URogueHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	TSubclassOf<UUpgradeEntryWidget> ItemBoxClass;

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
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* DistanceTraveledText;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URogueCardSelection> CardSelectionClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<URogueCardSelection> RelicSelectionClass;

	UPROPERTY()
	URogueCardSelection* CardSelection;
	UPROPERTY()
	URogueCardSelection* RelicSelection;

	void SetLevel(int _level, float _progress);
	void SetDistanceText(int _distance);
	void SetPressure(BigNumber _current, BigNumber _max);
	UUpgradeEntryWidget* AddItemWidget(URogueItem* _item);
};
