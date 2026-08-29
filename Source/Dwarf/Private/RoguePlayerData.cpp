// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguePlayerData.h"
#include "Components/RichTextBlock.h"
#include "DwarfPawn.h"
#include "RogueHUD.h"

void URoguePlayerData::Update(float _dt)
{
	if (timePaused) return;

	// Take pressure damage
	BigNumber PressureDamage = currentCave->first->Data.pressureValue * _dt * (1.0f - PressureResistance);
	OnPressureCalc.Broadcast(PressureDamage);
	Pressure -= PressureDamage;
	if (Pressure <= 0) // Lose
	{
		//RunRewards();
		return;
	}

	RogueHUD->SetPressure(Pressure, MaxPressure);

	// Run auto damagers
	clickTimer -= _dt;
	if (clickTimer <= 0)
	{
		currentCave->dwarfPawn->HitAnimation();
		clickTimer = GetHitCooldown();

		BigNumber damage = GetClickDamage();
		damage *= ClickSource.BlockDamageMultiplier[currentCave->first->Data.type];
		currentCave->DamageFirst(damage, ClickSource);
	}

	OnTick.Broadcast(_dt);
}

void URoguePlayerData::RogueCardsPopup()
{
	TArray<int> tempPool;
	for (int i = 0; i < currentItemPool.Num(); i++) tempPool.Add(i);

	if (currentItemPool.Num() < 3) return; // TODO: Flat stat increase

	// Item TODO: Create actual item random (with at least 1 level up, rarity weight, and max level restriction)
	int randIndex = rand() % tempPool.Num();
	itemsSelection[0] = tempPool[randIndex];
	tempPool.RemoveAt(randIndex);
	RogueHUD->CardSelection->CardLeft->SetItemInfo(currentItemPool[itemsSelection[0]]);

	randIndex = rand() % tempPool.Num();
	itemsSelection[1] = tempPool[randIndex];
	tempPool.RemoveAt(randIndex);
	RogueHUD->CardSelection->CardMiddle->SetItemInfo(currentItemPool[itemsSelection[1]]);

	randIndex = rand() % tempPool.Num();
	itemsSelection[2] = tempPool[randIndex];
	tempPool.RemoveAt(randIndex);
	RogueHUD->CardSelection->CardRight->SetItemInfo(currentItemPool[itemsSelection[2]]);

	Pause();
	RogueHUD->CardSelection->SetVisibility(ESlateVisibility::Visible);
}

void URoguePlayerData::BlockReward(BlockData _data)
{
	IncreaseExp(GetExp(_data.expValue));

	RegeneratePressure(_data.pressureRegen);

	RogueHUD->SetPressure(Pressure, MaxPressure);
	RogueHUD->SetDistanceText(currentCave->first->Data.index / 4);
}

void URoguePlayerData::IncreaseExp(BigNumber _value)
{
	experience += _value;

	while (experience >= experienceRequired)
	{
		LevelUpRogue();
	}
	RogueHUD->SetLevel(level, (float)experience / (float)experienceRequired);
}

void URoguePlayerData::AddItem(URogueItem* _item)
{
	// Check if Item already exists
	int itemIndex = items.Find(_item);
	if (itemIndex != INDEX_NONE)
	{
		_item->level++;
		_item->OnLevelUp();

		UUpgradeEntryWidget* widget = itemWidgets[itemIndex];
		widget->level++;
		widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
		widget->descriptionText = _item->GetDescriptionText(_item->level);
	}

	else
	{
		// New item: Bind and add widget
		_item->Bind(this, currentCave);
		_item->level = 1;
		_item->OnLevelUp();
		items.Add(_item);
		itemWidgets.Add(RogueHUD->AddItemWidget(_item));
	}
}

void URoguePlayerData::AddItemFromPool(unsigned int _index)
{
	AddItem(currentItemPool[_index]);
	if (currentItemPool[_index]->level >= MAX_ITEM_LEVEL)
	{
		currentItemPool.RemoveAt(_index);
	}
}

void URoguePlayerData::SelectItem(int _index)
{
	Resume();
	RogueHUD->CardSelection->SetVisibility(ESlateVisibility::Hidden);
	AddItemFromPool(itemsSelection[_index]);
}

BigNumber URoguePlayerData::GetExp(BigNumber _exp)
{
	BigNumber expValue = _exp;
	// Call delegate
	OnExpCalc.Broadcast(expValue);
	return expValue;
}

void URoguePlayerData::LevelUpRogue()
{
	RogueCardsPopup();

	level++;
	experience -= experienceRequired;
	experienceRequired *= 1.4f;
}

float URoguePlayerData::GetHitCooldown()
{
	float cd = clickCooldown;
	OnCooldownCalc.Broadcast(cd);
	return cd;
}

BigNumber URoguePlayerData::GetClickDamage()
{
	BigNumber DamageDelta = MaxDamage - MinDamage;
	BigNumber DamageBonus = rand() % ((int)DamageDelta + 1);

	// Damage Items //
	BigNumber damage = MinDamage + DamageBonus;
	OnDamageCalc.Broadcast(damage);

	return damage;
}

void URoguePlayerData::RegeneratePressure(BigNumber _baseValue)
{
	OnPressureRegenCalc.Broadcast(_baseValue);
	Pressure += _baseValue;
	if (Pressure > MaxPressure) Pressure = MaxPressure;
}
