#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/HorizontalBox.h"
#include "ResourceEntryWidget.h"
#include "ResourceEntryData.h"
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

protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UListView* ResourceList;

	UPROPERTY()
	FString ResourceNames[3];

	UPROPERTY()
	TArray<UResourceEntryData*> ResourceItems;
};
