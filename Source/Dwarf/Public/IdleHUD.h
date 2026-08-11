#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/HorizontalBox.h"
#include "Components/WrapBox.h"
#include "Components/Image.h"

#include "ResourceEntryWidget.h"
#include "ResourceEntryData.h"
#include "AutoAttackerDisplay.h"
#include "UpgradeEntryWidget.h"
#include "BigNumber.h"

#include "Block.h"

#include "IdleHUD.generated.h"


UCLASS()
class DWARF_API UIdleHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetResource(int _type, BigNumber _value);
	void AddResource(int _type, BigNumber _value);
	void Populate();

	void UpdateResources(BigNumber (&_resources)[RESOURCE_COUNT]);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* SaveButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* MenuButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* RebirthButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* RelicBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* UpgradeBox;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUpgradeEntryWidget* ClickUpgradeBox;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUpgradeEntryWidget* DrillUpgradeBox;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUpgradeEntryWidget* BoomUpgradeBox;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUpgradeEntryWidget* CritUpgradeBox;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUpgradeEntryWidget* YieldUpgradeBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* CharacterMenuButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UExpBarWidget* ExpBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UAutoAttackerDisplay* DrillDisplay;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UAutoAttackerDisplay* BoomDisplay;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TechPointIndicator;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* ButtonMult1;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* ButtonMult5;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* ButtonMult10;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* ButtonMult25;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* ButtonMult100;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* ButtonMultMax;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* DistanceTraveledText;

protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UListView* ResourceList;

	UPROPERTY()
	TArray<UResourceEntryData*> ResourceItems;
};
