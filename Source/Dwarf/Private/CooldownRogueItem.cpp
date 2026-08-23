#include "CooldownRogueItem.h"
#include "DwarfPlayerState.h"

void UCooldownRogueItem::CooldownCalc(float& _cd)
{
	_cd *= powf(0.90f, level);
}

void UCooldownRogueItem::Bind(URoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnCooldownCalc.AddUObject(this, &UCooldownRogueItem::CooldownCalc);
}

void UCooldownRogueItem::UnBind(URoguePlayerData* _player, Cave* _cave)
{
	_player->OnCooldownCalc.Remove(handle);
}

FString UCooldownRogueItem::GetDescriptionText(int _level)
{
	return "Increases your basic hit's attack speed by " + FString::SanitizeFloat(1.0f/powf(0.90f, _level)) + ". \n\"We don't have all day!\"";
}