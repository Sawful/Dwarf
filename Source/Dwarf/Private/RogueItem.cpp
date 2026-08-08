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
	_damage *= powf(2, level);
}

void MultihitRogueItem::CooldownCalc(float& _cd)
{
	if (reloading)
	{
		reloading = false;
		hitCounter = level;
		return;
	}

	_cd *= 0.01f;

	hitCounter--;
	if (hitCounter <= 0)
	{
		reloading = true;
	}
}

void MultihitRogueItem::Bind(RoguePlayerData* _player)
{
	_player->items.Add(this);
	handle = _player->OnCooldownCalc.AddRaw(this, &MultihitRogueItem::CooldownCalc);
}

void MultihitRogueItem::UnBind(RoguePlayerData* _player)
{
	_player->OnDamageCalc.Remove(handle);
}

void CooldownRogueItem::CooldownCalc(float& _cd)
{
	_cd *= powf(0.90f, level);
	}

void CooldownRogueItem::Bind(RoguePlayerData* _player)
{
	_player->items.Add(this);
	handle = _player->OnCooldownCalc.AddRaw(this, &CooldownRogueItem::CooldownCalc);
}

void CooldownRogueItem::UnBind(RoguePlayerData* _player)
{
	_player->OnDamageCalc.Remove(handle);
}