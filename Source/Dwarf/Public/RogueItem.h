#pragma once

#include "DamageSource.h"

#define MAX_ITEM_LEVEL 7

struct RoguePlayerData;
class Cave;
class IdleRelic;

enum ItemRarity
{
	COMMON = 0,
	UNCOMMON,
	RARE,
	EPIC,
	LEGENDARY
};

FSlateColor GetRarityColor(ItemRarity _rarity);

class RogueItem
{
public:
	virtual ~RogueItem() {};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave) = 0;
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave) = 0;
	virtual FString GetDescriptionText(int _level) = 0;
	virtual void OnLevelUp() {};
	virtual int GetRelicWeight();
	int level = 0;
	ItemRarity rarity;
	FString name;
	UTexture2D* icon;
	IdleRelic* associatedRelic = nullptr;
};

class DamageRogueItem : public RogueItem
{
	void DamageCalc(BigNumber& _damage);
	FDelegateHandle handle;
public:
	DamageRogueItem() { 
		rarity = COMMON; 
		name = "Sharpening stone"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/StrongArms.StrongArms")); 
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
};

class MultihitRogueItem : public RogueItem
{
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;

	bool reloading = false;
	int hitCounter;
public:
	MultihitRogueItem() { 
		rarity = UNCOMMON; 
		name = "Another pickaxe"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/pick_04.pick_04"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
};

class CooldownRogueItem : public RogueItem
{
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;
public:
	CooldownRogueItem() { 
		rarity = COMMON; 
		name = "Desynchronized clock";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/ArrowUp.ArrowUp"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
};

class ResistanceRogueItem : public RogueItem
{
	void PressureCalc(BigNumber& _pressure);
	float GetMult(int _level);
	float multiplier;

	FDelegateHandle handle;
public:
	ResistanceRogueItem() {
		rarity = UNCOMMON; 
		name = "Pressure Resistance";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/BlueShield.BlueShield"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
	virtual void OnLevelUp();
};

class RegenRogueItem : public RogueItem
{
	void PressureRegenCalc(BigNumber& _pressure);
	BigNumber GetMult(int _level);
	BigNumber multiplier;

	FDelegateHandle handle;
public:
	RegenRogueItem() {
		rarity = COMMON; 
		name = "Pressure Regen";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/DumbHeart.DumbHeart"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
	virtual void OnLevelUp();
};

class DrillRogueItem : public RogueItem
{
	void OnTick(float _dt);
	FDelegateHandle handle;

	int damage;
	float cooldown = 2.0f;
	float clock = 2.0f;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	DamageSource source;

	Cave* cave;

public:
	DrillRogueItem() {
		rarity = COMMON;
		name = "Magic-Forged Drill";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Drill.Drill"));
		source.TextType = AUTO;
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
	virtual void OnLevelUp();
};

class TNTRogueItem : public RogueItem
{
	void OnTick(float _dt);
	FDelegateHandle handle;

	int damage;
	float cooldown = 2.0f;
	float clock = 2.0f;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	DamageSource source;

	Cave* cave;

public:
	TNTRogueItem() {
		rarity = UNCOMMON;
		name = "Magic-Forged TNT";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/TNT.TNT"));
		source.TextType = AUTO;
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
	virtual void OnLevelUp();
};

class EarthquakeRogueItem : public RogueItem
{
	void OnTick(float _dt);
	FDelegateHandle handle;

	int damage;
	float cooldown = 2.0f;
	float clock = 2.0f;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	DamageSource source;

	Cave* cave;

public:
	EarthquakeRogueItem() {
		rarity = RARE; 
		name = "Magic-Forged Totem"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Totem.Totem"));
		source.TextType = AUTO;
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
	virtual void OnLevelUp();
};

class LaserRogueItem : public RogueItem
{
	void OnTick(float _dt);
	FDelegateHandle handle;

	int damage;
	float cooldown = 2.0f;
	float clock = 2.0f;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	DamageSource source;

	Cave* cave;

public:
	LaserRogueItem() {
		rarity = EPIC;
		name = "Magic-Forged Laser";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Laser.Laser"));
		source.TextType = AUTO;
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
	virtual void OnLevelUp();
};