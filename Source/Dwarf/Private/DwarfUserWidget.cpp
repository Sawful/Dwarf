#include "DwarfUserWidget.h"

void UDwarfUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDwarfUserWidget::Populate()
{
	ResourceList->ClearListItems();
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		UResourceEntryData* Item = NewObject<UResourceEntryData>(this);
		Item->ResourceName = GetResourceName((ResourceType)i);
		Item->Amount = 0;
		ResourceItems.Add(Item);
		ResourceList->AddItem(Item);
	}
}

void UDwarfUserWidget::UpdateResources(int(&_resources)[RESOURCE_COUNT])
{
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		ResourceItems[i]->SetAmount(_resources[i]);
	}
}

void UDwarfUserWidget::SetResource(int _type, int _value)
{
	ResourceItems[_type]->SetAmount(_value);
}

void UDwarfUserWidget::AddResource(int _type, int _value)
{
	ResourceItems[_type]->AddAmount(_value);
}
