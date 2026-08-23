#include "LaserRogueItem.h"
#include "DwarfPlayerState.h"
#include "Cave.h"

void ULaserRogueItem::Bind(URoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnTick.AddUObject(this, &ULaserRogueItem::OnTick);
	cave = _cave;
}

void ULaserRogueItem::UnBind(URoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void ULaserRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageRow(damage, source);
	}
}

int ULaserRogueItem::GetDamage(int _level)
{
	switch (_level)
	{
	case 1: return 50;
	case 2: return 100;
	case 3: return 150;
	case 4: return 200;
	case 5: return 250;
	case 6: return 300;
	case 7: return 500;
	}
	return 50;
}

float ULaserRogueItem::GetCooldown(int _level)
{
	switch (_level)
	{
	case 1: return 1.0f;
	case 2: return 0.9f;
	case 3: return 0.8f;
	case 4: return 0.7f;
	case 5: return 0.6f;
	case 6: return 0.5f;
	case 7: return 0.25f;
	}
	return 1.0f;
}

void ULaserRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString ULaserRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s to the lowest row of blocks. \n\"Pierces through magic like butter!\"";
}