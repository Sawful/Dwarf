#include "IdleHUD.h"

void UIdleHUD::NativeConstruct()
{
	Super::NativeConstruct();
	TechPointIndicator->SetVisibility(ESlateVisibility::Hidden);
}

void UIdleHUD::Populate()
{
	ResourceList->ClearListItems();
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		UResourceEntryData* Item = NewObject<UResourceEntryData>(this);
		Item->ResourceIcon = GetResourceIcon((ResourceType)i);
		Item->Amount = 0;
		ResourceItems.Add(Item);
		ResourceList->AddItem(Item);
	}
}

void UIdleHUD::UpdateResources(int(&_resources)[RESOURCE_COUNT])
{
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		ResourceItems[i]->SetAmount(_resources[i]);
	}
}

void UIdleHUD::SetResource(int _type, int _value)
{
	ResourceItems[_type]->SetAmount(_value);
}

void UIdleHUD::AddResource(int _type, int _value)
{
	ResourceItems[_type]->AddAmount(_value);
}
