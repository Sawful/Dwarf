#include "IdleHUD.h"
#include "Components/RichTextBlock.h"
#include "IdleRelic.h"

void UIdleHUD::NativeConstruct()
{
	Super::NativeConstruct();
	TechPointIndicator->SetVisibility(ESlateVisibility::Hidden);
}

void UIdleHUD::Populate()
{
	ResourceList->ClearListItems();
	ResourceItems.Empty(RESOURCE_COUNT);
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		UResourceEntryData* Item = NewObject<UResourceEntryData>(this);
		Item->ResourceIcon = GetResourceIcon((ResourceType)i);
		Item->Amount = 0;
		ResourceItems.Add(Item);
		ResourceList->AddItem(Item);
	}
	TakeWidget();
}

void UIdleHUD::UpdateResources(BigNumber(&_resources)[RESOURCE_COUNT])
{
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		if (_resources[i] == 0) continue;
		ResourceItems[i]->SetAmount(_resources[i]);
	}
}

UUpgradeEntryWidget* UIdleHUD::AddRelicWidget(IdleRelic* _relic)
{
	// Add to box
	UUpgradeEntryWidget* widget = CreateWidget<UUpgradeEntryWidget, UWrapBox*>(RelicBox, ItemBoxClass);
	widget->level = _relic->rank;
	widget->name = _relic->name;

	if (!IsValid(_relic->icon))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Icon invalid");
	}

	widget->Icon->SetBrushFromTexture(_relic->icon);
	widget->SetPadding(16);
	widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
	widget->descriptionText = _relic->GetDescriptionText(_relic->rank);
	RelicBox->AddChild(widget);

	return widget;
}

void UIdleHUD::SetResource(int _type, BigNumber _value)
{
	ResourceItems[_type]->SetAmount(_value);
}

void UIdleHUD::AddResource(int _type, BigNumber _value)
{
	ResourceItems[_type]->AddAmount(_value);
}
