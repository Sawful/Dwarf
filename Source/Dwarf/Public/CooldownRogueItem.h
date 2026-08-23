#pragma once

#include "RogueItem.h"
#include "CooldownRogueItem.generated.h"

UCLASS()
class UCooldownRogueItem : public URogueItem
{
	GENERATED_BODY()
	void CooldownCalc(float& _cd);
	FDelegateHandle handle;
public:
	UCooldownRogueItem() { 
		rarity = COMMON; 
		name = "Desynchronized clock";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/ArrowUp.ArrowUp"));
	};
	virtual void Bind(URoguePlayerData* _player, Cave* _cave) override;
	virtual void UnBind(URoguePlayerData* _player, Cave* _cave) override;
	virtual FString GetDescriptionText(int _level) override;
};


