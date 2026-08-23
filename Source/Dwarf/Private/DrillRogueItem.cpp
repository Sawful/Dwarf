#include "DrillRogueItem.h"
#include "DwarfPlayerState.h"
#include "Cave.h"

void UDrillRogueItem::Bind(URoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnTick.AddUObject(this, &UDrillRogueItem::OnTick);
	cave = _cave;
}

void UDrillRogueItem::UnBind(URoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void UDrillRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageFirst(damage, source);
	}
}

int UDrillRogueItem::GetDamage(int _level)
{
	switch (_level)
	{
	case 1: return 20;
	case 2: return 50;
	case 3: return 90;
	case 4: return 150;
	case 5: return 250;
	case 6: return 375;
	case 7: return 500;
	}
	return 20;
}

float UDrillRogueItem::GetCooldown(int _level)
{
	switch (_level)
	{
	case 1: return 2.0f;
	case 2: return 1.8f;
	case 3: return 1.5f;
	case 4: return 1.2f;
	case 5: return 1.0f;
	case 6: return 0.8f;
	case 7: return 0.6f;
	}
	return 2.0f;
}

void UDrillRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString UDrillRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s. \nIt looks just like your good old drill.";
}