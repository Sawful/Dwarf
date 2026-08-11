#include "RogueItem.h"
#include "DwarfPlayerState.h"
#include "IdleRelic.h"



void DamageRogueItem::Bind(RoguePlayerData* _player)
{
	_player->items.Add(this);
	handle = _player->OnDamageCalc.AddRaw(this, &DamageRogueItem::DamageCalc);
}

void DamageRogueItem::UnBind(RoguePlayerData* _player)
{
	_player->OnDamageCalc.Remove(handle);
}

int RogueItem::GetRelicWeight()
{
	switch (rarity)
	{
	case COMMON: return 25 * level;
	case UNCOMMON: return 20 * level;
	case RARE: return 15 * level;
	case EPIC: return 10 * level;
	case LEGENDARY: return 5 * level;
	default: return 0;
	}
}

void DamageRogueItem::DamageCalc(int& _damage)
{
	_damage *= powf(2, level);
}

FString DamageRogueItem::GetDescriptionText(int _level)
{
	return "The damage you deal is multiplied by x" + FString::FromInt(powf(2, _level)) + ".";
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

FString MultihitRogueItem::GetDescriptionText(int _level)
{
	return "Everytime you hit, hit " + FString::FromInt(_level) + " more time(s).\n\"What if I just attach it to mine?\"";
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

FString CooldownRogueItem::GetDescriptionText(int _level)
{
	return "Increases your attack speed by " + FString::SanitizeFloat(1.0f/powf(0.90f, _level)) + ". \n\"We don't have all day!\"";
}