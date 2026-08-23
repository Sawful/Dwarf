#include "TNTRogueItem.h"
#include "DwarfPlayerState.h"
#include "Cave.h"

void UTNTRogueItem::Bind(URoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnTick.AddUObject(this, &UTNTRogueItem::OnTick);
	cave = _cave;
}

void UTNTRogueItem::UnBind(URoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void UTNTRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageFirstColumn(damage, source);
	}
}

int UTNTRogueItem::GetDamage(int _level)
{
	switch (_level)
	{
	case 1: return 50;
	case 2: return 125;
	case 3: return 250;
	case 4: return 400;
	case 5: return 600;
	case 6: return 800;
	case 7: return 1000;
	}
	return 50;
}

float UTNTRogueItem::GetCooldown(int _level)
{
	switch (_level)
	{
	case 1: return 8.0f;
	case 2: return 7.5f;
	case 3: return 7.0f;
	case 4: return 6.5f;
	case 5: return 6.0f;
	case 6: return 5.5f;
	case 7: return 5.0f;
	}
	return 8.0f;
}

void UTNTRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString UTNTRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s to the nearest column of blocks. \n\"Goes boom\".";
}