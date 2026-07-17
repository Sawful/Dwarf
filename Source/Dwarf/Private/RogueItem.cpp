#include "RogueItem.h"
#include "DwarfPlayerState.h"

void DamageRogueItem::Bind(RoguePlayerData* _player)
{
	_player->items.Add(this);
	handle = _player->OnDamageCalc.AddRaw(this, &DamageRogueItem::DamageCalc);
}

void DamageRogueItem::UnBind(RoguePlayerData* _player)
{
	_player->OnDamageCalc.Remove(handle);
}

void DamageRogueItem::DamageCalc(int& _damage)
{
	_damage *= 2;
}
