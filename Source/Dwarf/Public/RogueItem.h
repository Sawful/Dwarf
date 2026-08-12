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
	void DamageCalc(int& _damage);
	FDelegateHandle handle;
public:
	DamageRogueItem() { 
		rarity = COMMON; 
		name = "Sharpening stone"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541")); 
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
};

class MultihitRogueItem : public RogueItem
{
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;

	bool reloading = true;
	int hitCounter;
public:
	MultihitRogueItem() { 
		rarity = UNCOMMON; 
		name = "Another pickaxe"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));
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
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
};

class DrillRogueItem : public RogueItem
{
	void OnTick(float _dt);
	FDelegateHandle handle;

	int damage;
	float cooldown;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	float clock;
	DamageSource source;

	Cave* cave;

public:
	DrillRogueItem() {
		rarity = COMMON;
		name = "Magic-Forged Drill";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));
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
	float cooldown;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	float clock;
	DamageSource source;

	Cave* cave;

public:
	TNTRogueItem() {
		rarity = UNCOMMON;
		name = "Magic-Forged TNT";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));
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
	float cooldown;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	float clock;
	DamageSource source;

	Cave* cave;

public:
	EarthquakeRogueItem() {
		rarity = RARE; 
		name = "Magic-Forged Totem"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));
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
	float cooldown;
	int GetDamage(int _level);
	float GetCooldown(int _level);
	float clock;
	DamageSource source;

	Cave* cave;

public:
	LaserRogueItem() {
		rarity = EPIC;
		name = "Magic-Forged Laser";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));
		source.TextType = AUTO;
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave);
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave);
	virtual FString GetDescriptionText(int _level);
	virtual void OnLevelUp();
};