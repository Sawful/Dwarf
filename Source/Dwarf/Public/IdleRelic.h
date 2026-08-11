// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RogueItem.h"
#include "UpgradeEntryWidget.h"

#define MAX_RANK 5 

class ADwarfPlayerState;

enum RelicTag
{
	DAMAGE,
	MULTIHIT,

	RELIC_COUNT
};

class DWARF_API IdleRelic
{
public:
	IdleRelic();
	virtual ~IdleRelic() {};

	virtual void Bind(ADwarfPlayerState* _player) {};
	virtual void UnBind(ADwarfPlayerState* _player) {};
	virtual FString GetDescriptionText(int _level) = 0;
	virtual void RankUp();
	virtual void IncreaseCount(int _count);
	virtual int GetRequiredCount();
	virtual FString GetRankText();
	int rank = 0;
	int cumulativeCount = 0;
	int count = 0;
	int countRequired;
	ItemRarity rarity;
	FString name;
	UTexture2D* icon;
	RelicTag tag;
	UUpgradeEntryWidget* widget = nullptr;
};

class DamageIdleRelic : public IdleRelic
{
	void DamageCalc(int& _damage);
	FDelegateHandle handle;
	float multiplier;
	int GetMult(int _level);
public:
	DamageIdleRelic() { rarity = COMMON; name = "Sharpening stone"; tag = DAMAGE; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541")); };
	virtual void Bind(ADwarfPlayerState* _player);
	virtual void UnBind(ADwarfPlayerState* _player);
	virtual void RankUp();
	virtual FString GetDescriptionText(int _level);
};

class MultihitIdleRelic : public IdleRelic
{
	void OnHit(ADwarfPlayerState* _player);
	FDelegateHandle handle;

	bool reloading = true;
	int hitCounter;
public:
	MultihitIdleRelic() { rarity = UNCOMMON; name = "Rusty pickaxe"; tag = MULTIHIT; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541")); };
	virtual void Bind(ADwarfPlayerState* _player);
	virtual void UnBind(ADwarfPlayerState* _player);
	virtual FString GetDescriptionText(int _level);
};