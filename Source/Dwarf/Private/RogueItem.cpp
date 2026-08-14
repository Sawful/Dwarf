#include "RogueItem.h"
#include "DwarfPlayerState.h"
#include "IdleRelic.h"
#include "Cave.h"

void RegenRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnPressureRegenCalc.AddRaw(this, &RegenRogueItem::PressureRegenCalc);
}
void RegenRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	handle.Reset();
}

void RegenRogueItem::PressureRegenCalc(BigNumber& _pressure)
{
	_pressure *= multiplier;
}

BigNumber RegenRogueItem::GetMult(int _level)
{
	switch(_level)
	{ 
	case 1: return 2.0f;
	case 2: return 3.0f;
	case 3: return 4.0f;
	case 4: return 5.5f;
	case 5: return 7.0f;
	case 6: return 8.5f;
	case 7: return 10.0f;
	}

	return 1.0f;
}

void RegenRogueItem::OnLevelUp()
{
	multiplier = GetMult(level);
}
FString RegenRogueItem::GetDescriptionText(int _level)
{
	return "Increases the pressure you gain when breaking blocks by +" + (((BigNumber)1.0f / (GetMult(_level) - 1)) * 100.0f).ToStringTrunc() + "%.";
}

void ResistanceRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnPressureCalc.AddRaw(this, &ResistanceRogueItem::PressureCalc);
}
void ResistanceRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	handle.Reset();
}

void ResistanceRogueItem::PressureCalc(BigNumber& _pressure)
{
	_pressure *= multiplier;
}

float ResistanceRogueItem::GetMult(int _level)
{
	switch (_level)
	{
	case 1: return 0.9f;
	case 2: return 0.8f;
	case 3: return 0.7f;
	case 4: return 0.55f;
	case 5: return 0.4f;
	case 6: return 0.25f;
	case 7: return 0.1f;
	}

	return 1.0f;
}
void ResistanceRogueItem::OnLevelUp()
{
	multiplier = GetMult(level);
}
FString ResistanceRogueItem::GetDescriptionText(int _level)
{
	return "Increases your resistance to pressure by +" + FString::FromInt((1.0f / (GetMult(_level) - 1)) * 100) + "%.";
}


void DamageRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnDamageCalc.AddRaw(this, &DamageRogueItem::DamageCalc);
}

void DamageRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
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

void DamageRogueItem::DamageCalc(BigNumber& _damage)
{
	_damage *= powf(2, level);
}

FString DamageRogueItem::GetDescriptionText(int _level)
{
	return "The damage your base hit deals is multiplied by x" + FString::FromInt(powf(2, _level)) + ".";
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

void MultihitRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnCooldownCalc.AddRaw(this, &MultihitRogueItem::CooldownCalc);
}

void MultihitRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnCooldownCalc.Remove(handle);
}

FString MultihitRogueItem::GetDescriptionText(int _level)
{
	return "Everytime you hit, hit " + FString::FromInt(_level) + " more time(s).\n\"What if I just attach it to mine?\"";
}

void CooldownRogueItem::CooldownCalc(float& _cd)
{
	_cd *= powf(0.90f, level);
}

void CooldownRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnCooldownCalc.AddRaw(this, &CooldownRogueItem::CooldownCalc);
}

void CooldownRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnCooldownCalc.Remove(handle);
}

FString CooldownRogueItem::GetDescriptionText(int _level)
{
	return "Increases your basic hit's attack speed by " + FString::SanitizeFloat(1.0f/powf(0.90f, _level)) + ". \n\"We don't have all day!\"";
}

void DrillRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnTick.AddRaw(this, &DrillRogueItem::OnTick);
	cave = _cave;
}

void DrillRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void DrillRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageFirst(damage, source);
	}
}

int DrillRogueItem::GetDamage(int _level)
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

float DrillRogueItem::GetCooldown(int _level)
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

void DrillRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString DrillRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s. \nIt looks just like your good old drill.";
}

void TNTRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnTick.AddRaw(this, &TNTRogueItem::OnTick);
	cave = _cave;
}

void TNTRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void TNTRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageFirstColumn(damage, source);
	}
}

int TNTRogueItem::GetDamage(int _level)
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

float TNTRogueItem::GetCooldown(int _level)
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

void TNTRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString TNTRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s to the nearest column of blocks. \n\"Goes boom\".";
}

void EarthquakeRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnTick.AddRaw(this, &EarthquakeRogueItem::OnTick);
	cave = _cave;
}

void EarthquakeRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void EarthquakeRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageArea(damage, source);
	}
}

int EarthquakeRogueItem::GetDamage(int _level)
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

float EarthquakeRogueItem::GetCooldown(int _level)
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

void EarthquakeRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString EarthquakeRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s to three layers of blocks. \n\"Bring it all down!\"";
}

void LaserRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnTick.AddRaw(this, &LaserRogueItem::OnTick);
	cave = _cave;
}

void LaserRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnTick.Remove(handle);
}

void LaserRogueItem::OnTick(float _dt)
{
	clock -= _dt;
	while (clock <= 0)
	{
		clock += cooldown;
		cave->DamageRow(damage, source);
	}
}

int LaserRogueItem::GetDamage(int _level)
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

float LaserRogueItem::GetCooldown(int _level)
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

void LaserRogueItem::OnLevelUp()
{
	damage = GetDamage(level);
	cooldown = GetCooldown(level);
	clock = cooldown;
}

FString LaserRogueItem::GetDescriptionText(int _level)
{
	return "Deals " + FString::FromInt(GetDamage(_level)) + " dmg every " + FString::SanitizeFloat(GetCooldown(_level)) + "s to the lowest row of blocks. \n\"Pierces through magic like butter!\"";
}

FSlateColor GetRarityColor(ItemRarity _rarity)
{
	switch(_rarity)
	{
		case COMMON:
		{
			return FSlateColor(FLinearColor(0.9, 0.9, 0.9, 1));
		}
		case UNCOMMON:
		{
			return FSlateColor(FLinearColor(0.2, 0.9, 0.2, 1));
		}
		case RARE:
		{
			return FSlateColor(FLinearColor(0.2, 0.2, 0.9, 1));
		}
		case EPIC:
		{
			return FSlateColor(FLinearColor(0.7, 0.2, 0.9, 1));
		}
		case LEGENDARY:
		{
			return FSlateColor(FLinearColor(0.9, 0.7, 0.2, 1));
		}
	}
	return FSlateColor();
}
