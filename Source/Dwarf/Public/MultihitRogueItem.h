#pragma once

#include "RogueItem.h"
#include "MultihitRogueItem.generated.h"

UCLASS()
class UMultihitRogueItem : public URogueItem
{
	GENERATED_BODY()
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;

	bool reloading = false;
	int hitCounter;
public:
	UMultihitRogueItem() { 
		rarity = UNCOMMON; 
		name = "Another pickaxe"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/pick_04.pick_04"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave) override;
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave) override;
	virtual FString GetDescriptionText(int _level) override;
};


