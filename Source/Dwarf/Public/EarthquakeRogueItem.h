#pragma once

#include "RogueItem.h"
#include "EarthquakeRogueItem.generated.h"

UCLASS()
class UEarthquakeRogueItem : public URogueItem
{
	GENERATED_BODY()
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
	UEarthquakeRogueItem() {
		rarity = RARE; 
		name = "Magic-Forged Totem"; 
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Totem.Totem"));
		source.TextType = AUTO;
	};
	virtual void Bind(RoguePlayerData* _player, Cave* _cave) override;
	virtual void UnBind(RoguePlayerData* _player, Cave* _cave) override;
	virtual FString GetDescriptionText(int _level) override;
	virtual void OnLevelUp();
};