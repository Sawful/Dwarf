// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RogueItem.h"
#include "UpgradeEntryWidget.h"

#define MAX_RANK 5 

class ADwarfPlayerState;
class Cave;

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

	virtual void Bind(ADwarfPlayerState* _player, Cave* _cave) = 0;
	virtual void UnBind(ADwarfPlayerState* _player, Cave* _cave) = 0;
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
	void DamageCalc(BigNumber& _damage);
	FDelegateHandle handle;
	float multiplier;
	int GetMult(int _level);
public:
	DamageIdleRelic() { rarity = COMMON; name = "Sharpening stone"; tag = DAMAGE; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/StrongArms.StrongArms")); };
	virtual void Bind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void UnBind(ADwarfPlayerState* _player, Cave* _cave);
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
	MultihitIdleRelic() { rarity = UNCOMMON; name = "Rusty pickaxe"; tag = MULTIHIT; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/pick_04.pick_04")); };
	virtual void Bind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void UnBind(ADwarfPlayerState* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
};

class DrillIdleRelic : public IdleRelic
{
	void DamageCalc(BigNumber& _damage);
	void CooldownCalc(float& _cd);
	FDelegateHandle handleDmg;
	FDelegateHandle handleCd;
	float damageMultiplier;
	float cooldownMultiplier;
	int GetDamageMult(int _level);
	int GetCooldownMult(int _level);
public:
	DrillIdleRelic() { rarity = COMMON; name = "Drill Essence"; tag = DAMAGE; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Drill.Drill")); };
	virtual void Bind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void UnBind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void RankUp();
	virtual FString GetDescriptionText(int _level);
};

class BoomIdleRelic : public IdleRelic
{
	void DamageCalc(BigNumber& _damage);
	void CooldownCalc(float& _cd);
	FDelegateHandle handleDmg;
	FDelegateHandle handleCd;
	float damageMultiplier;
	float cooldownMultiplier;
	int GetDamageMult(int _level);
	int GetCooldownMult(int _level);
public:
	BoomIdleRelic() { rarity = UNCOMMON; name = "Ethereal Gunpowder"; tag = DAMAGE; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/TNT.TNT")); };
	virtual void Bind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void UnBind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void RankUp();
	virtual FString GetDescriptionText(int _level);
};

class EarthquakeIdleRelic : public IdleRelic
{
	void DamageCalc(BigNumber& _damage);
	void CooldownCalc(float& _cd);
	FDelegateHandle handleDmg;
	FDelegateHandle handleCd;
	float damageMultiplier;
	float cooldownMultiplier;
	int GetDamageMult(int _level);
	int GetCooldownMult(int _level);
public:
	EarthquakeIdleRelic() { rarity = RARE; name = "Dwarven Charm"; tag = DAMAGE; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Totem.Totem")); };
	virtual void Bind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void UnBind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void RankUp();
	virtual FString GetDescriptionText(int _level);
};

class LaserIdleRelic : public IdleRelic
{
	void DamageCalc(BigNumber& _damage);
	void CooldownCalc(float& _cd);
	FDelegateHandle handleDmg;
	FDelegateHandle handleCd;
	float damageMultiplier;
	float cooldownMultiplier;
	int GetDamageMult(int _level);
	int GetCooldownMult(int _level);
public:
	LaserIdleRelic() { rarity = EPIC; name = "Amplifying Lens"; tag = DAMAGE; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Laser.Laser")); };
	virtual void Bind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void UnBind(ADwarfPlayerState* _player, Cave* _cave);
	virtual void RankUp();
	virtual FString GetDescriptionText(int _level);
};