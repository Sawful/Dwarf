#pragma once

#include "RogueItem.h"
#include "ResistanceRogueItem.generated.h"

UCLASS()
class UResistanceRogueItem : public URogueItem
{
	GENERATED_BODY()
	void PressureCalc(BigNumber& _pressure);
	float GetMult(int _level);
	float multiplier;

	FDelegateHandle handle;
public:
	UResistanceRogueItem() {
		rarity = UNCOMMON; 
		name = "Pressure Resistance";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/BlueShield.BlueShield"));
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave) override;
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave) override;
	virtual FString GetDescriptionText(int _level) override;
	virtual void OnLevelUp();
};


