#pragma once

#include "RogueItem.h"
#include "LaserRogueItem.generated.h"

UCLASS()
class ULaserRogueItem : public URogueItem
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
	ULaserRogueItem() {
		rarity = EPIC;
		name = "Magic-Forged Laser";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Laser.Laser"));
		source.TextType = AUTO;
	};
	virtual void Bind(URoguePlayerData* _player, Cave* _cave) override;
	virtual void UnBind(URoguePlayerData* _player, Cave* _cave) override;
	virtual FString GetDescriptionText(int _level) override;
	virtual void OnLevelUp();
};