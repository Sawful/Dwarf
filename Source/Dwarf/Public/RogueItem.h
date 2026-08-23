#pragma once

#include "DamageSource.h"
#include "CoreMinimal.h"
#include "RogueItem.generated.h"

#define MAX_ITEM_LEVEL 7

class URoguePlayerData;
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

UCLASS(Abstract)
class DWARF_API URogueItem : public UObject
{
	GENERATED_BODY()
public:
	virtual ~URogueItem() {};
	virtual void Bind(URoguePlayerData* _player, Cave* _cave) PURE_VIRTUAL(URogueItem::Bind, );
	virtual void UnBind(URoguePlayerData* _player, Cave* _cave) PURE_VIRTUAL(URogueItem::UnBind, );
	virtual FString GetDescriptionText(int _level) PURE_VIRTUAL(URogueItem::GetDescriptionText, return FString(););
	virtual void OnLevelUp() {};
	virtual int GetRelicWeight();
	int level = 0;
	ItemRarity rarity;
	FString name;

	UPROPERTY()
	UTexture2D* icon;

	IdleRelic* associatedRelic = nullptr;
};
