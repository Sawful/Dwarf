#pragma once

#include "RogueItem.h"
#include "DamageRogueItem.generated.h"

UCLASS()
class DWARF_API UDamageRogueItem : public URogueItem
{
	GENERATED_BODY()
	void DamageCalc(BigNumber& _damage);
	FDelegateHandle handle;
public:
	UDamageRogueItem() {
		rarity = COMMON;
		name = "Sharpening stone";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/StrongArms.StrongArms"));
	};
	virtual void Bind(URoguePlayerData* _player, Cave* _cave) override;
	virtual void UnBind(URoguePlayerData* _player, Cave* _cave) override;
	virtual FString GetDescriptionText(int _level) override;
};

