#include "DamageRogueItem.h"
#include "DwarfPlayerState.h"

void UDamageRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnDamageCalc.AddUObject(this, &UDamageRogueItem::DamageCalc);
}

void UDamageRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnDamageCalc.Remove(handle);
}

void UDamageRogueItem::DamageCalc(BigNumber& _damage)
{
	_damage *= powf(2, level);
}

FString UDamageRogueItem::GetDescriptionText(int _level)
{
	return "The damage your base hit deals is multiplied by x" + FString::FromInt(powf(2, _level)) + ".";
}