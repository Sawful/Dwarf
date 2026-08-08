#pragma once

#define MAX_ITEM_LEVEL 7

struct RoguePlayerData;

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
	int level = 0;
	ItemRarity rarity;
	FString name;
	FString description;
	UTexture2D* icon;
};

class DamageRogueItem : public RogueItem
{
	void DamageCalc(int& _damage);
	FDelegateHandle handle;
public:
	DamageRogueItem() { rarity = COMMON; name = "Sharpening stone"; description = "The damage you deal is multiplied by 2x."; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541")); };
	virtual void Bind(RoguePlayerData* _player);
	virtual void UnBind(RoguePlayerData* _player);
};

class MultihitRogueItem : public RogueItem
{
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;

	bool reloading = true;
	int hitCounter;
public:
	MultihitRogueItem() { rarity = COMMON; name = "Another pickaxe"; description = "Everytime you hit, hit a second time.\n\"What if I just attach it to mine?\""; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));};
	virtual void Bind(RoguePlayerData* _player);
	virtual void UnBind(RoguePlayerData* _player);
};

class CooldownRogueItem : public RogueItem
{
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;
public:
	CooldownRogueItem() { rarity = COMMON; name = "Desynchronized clock"; description = "Increases your attack speed. \n\"We don't have all day!\""; icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));};
	virtual void Bind(RoguePlayerData* _player);
	virtual void UnBind(RoguePlayerData* _player);
};