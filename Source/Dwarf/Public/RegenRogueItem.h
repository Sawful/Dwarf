#pragma once

#include "RogueItem.h"
#include "RegenRogueItem.generated.h"

UCLASS()
class URegenRogueItem : public URogueItem
{
	GENERATED_BODY()
	void PressureRegenCalc(BigNumber& _pressure);
	BigNumber GetMult(int _level);
	BigNumber multiplier;

	FDelegateHandle handle;
public:
	URegenRogueItem() {
		rarity = COMMON; 
		name = "Pressure Regen";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/DumbHeart.DumbHeart"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave) override;
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave) override;
	virtual FString GetDescriptionText(int _level) override;
	virtual void OnLevelUp();
};

