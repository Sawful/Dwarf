#pragma once

#define MAX_ITEM_LEVEL 7

struct RoguePlayerData;
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
	virtual void Bind(RoguePlayerData* _player) = 0;
	virtual void UnBind(RoguePlayerData* _player) = 0;
	virtual FString GetDescriptionText(int _level) = 0;
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
	DamageRogueItem() { rarity = COMMON; name = "Sharpening stone"; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541")); };
	virtual void Bind(RoguePlayerData* _player);
	virtual void UnBind(RoguePlayerData* _player);
	virtual FString GetDescriptionText(int _level);
};

class MultihitRogueItem : public RogueItem
{
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;

	bool reloading = true;
	int hitCounter;
public:
	MultihitRogueItem() { rarity = UNCOMMON; name = "Another pickaxe"; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));};
	virtual void Bind(RoguePlayerData* _player);
	virtual void UnBind(RoguePlayerData* _player);
	virtual FString GetDescriptionText(int _level);
};

class CooldownRogueItem : public RogueItem
{
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;
public:
	CooldownRogueItem() { rarity = COMMON; name = "Desynchronized clock"; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));};
	virtual void Bind(RoguePlayerData* _player);
	virtual void UnBind(RoguePlayerData* _player);
	virtual FString GetDescriptionText(int _level);
};