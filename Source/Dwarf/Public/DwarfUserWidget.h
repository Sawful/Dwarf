#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"

#include "ResourceEntryWidget.h"
#include "ResourceEntryData.h"
#include "ExpBarWidget.h"

#include "Block.h"

#include "DwarfUserWidget.generated.h"


UCLASS()
class DWARF_API UDwarfUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetResource(int _type, int _value);
	void AddResource(int _type, int _value);
	void Populate();

	void UpdateResources(int (&_resources)[RESOURCE_COUNT]);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UListView* UpgradeBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* SaveButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* MenuButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* RebirthButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* CharacterMenuButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UExpBarWidget* ExpBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TechPointIndicator;

protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UListView* ResourceList;

	UPROPERTY()
	TArray<UResourceEntryData*> ResourceItems;
};
