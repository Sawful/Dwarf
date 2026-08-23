#include "EarthquakeRogueItem.h"
#include "DwarfPlayerState.h"
#include "Cave.h"

void UEarthquakeRogueItem::Bind(URoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnTick.AddUObject(this, &UEarthquakeRogueItem::OnTick);
	cave = _cave;
}

void UEarthquakeRogueItem::UnBind(URoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void UEarthquakeRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageArea(damage, source);
	}
}

int UEarthquakeRogueItem::GetDamage(int _level)
{
	switch (_level)
	{
	case 1: return 50;
	case 2: return 100;
	case 3: return 150;
	case 4: return 200;
	case 5: return 250;
	case 6: return 300;
	case 7: return 400;
	}
	return 50;
}

float UEarthquakeRogueItem::GetCooldown(int _level)
{
	switch (_level)
	{
	case 1: return 5.0f;
	case 2: return 4.5f;
	case 3: return 4.0f;
	case 4: return 3.5f;
	case 5: return 3.0f;
	case 6: return 2.5f;
	case 7: return 2.0f;
	}
	return 5.0f;
}

void UEarthquakeRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString UEarthquakeRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s to three layers of blocks. \n\"Bring it all down!\"";
}