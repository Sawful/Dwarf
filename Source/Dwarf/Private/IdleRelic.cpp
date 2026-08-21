// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleRelic.h"
#include "DwarfPlayerState.h"
#include "Cave.h"

IdleRelic::IdleRelic() { countRequired = GetRequiredCount(); }

void IdleRelic::RankUp()
{
	count -= countRequired;
	rank++;
	countRequired = GetRequiredCount();
}

void IdleRelic::IncreaseCount(int _count)
{
	count += _count;
	cumulativeCount += _count;
	while (count >= countRequired)
	{
		RankUp();
	}
}

int IdleRelic::GetRequiredCount()
{
	return round(powf(rank + 1, 2));
}

FString IdleRelic::GetRankText()
{
	FString rankText;
	for (int i = 0; i < rank; i++) rankText += "1";
	for (int i = rank; i < MAX_RANK; i++) rankText += "0";
	return rankText;
}

void DamageIdleRelic::Bind(ADwarfPlayerState* _player, Cave* _cave)
{
	handle = _player->OnClickDamageCalc.AddRaw(this, &DamageIdleRelic::DamageCalc);
}

void DamageIdleRelic::UnBind(ADwarfPlayerState* _player, Cave* _cave)
{
	handle.Reset();
}

FString DamageIdleRelic::GetDescriptionText(int _level)
{
	return "Increase your click damage by +" + FString::FromInt((GetMult(_level) - 1) * 100) + "%.";
}

void DamageIdleRelic::RankUp()
{
	IdleRelic::RankUp();
	
	multiplier = GetMult(rank);
}

void DamageIdleRelic::DamageCalc(BigNumber& _damage)
{
	_damage *= multiplier;
}

int DamageIdleRelic::GetMult(int _level)
{
	switch (_level)
	{
	case 1: return 2; 
	case 2: return 5; 
	case 3: return 25;
	case 4: return 100;
	case 5: return 500;
	default: return 0;
	}
}

FString MultihitIdleRelic::GetDescriptionText(int _level)
{
	return "Every " + FString::FromInt(7 - _level) + " clicks hits twice.";
}

void MultihitIdleRelic::OnHit(ADwarfPlayerState* _player)
{
	if (reloading)
	{
		hitCounter--;
		if (hitCounter <= 0)
		{
			reloading = false;
		}
	}

	else
	{
		_player->Hit(true);
		reloading = true;
		hitCounter = 6 - rank;
	}
}

void MultihitIdleRelic::Bind(ADwarfPlayerState* _player, Cave* _cave)
{
	handle = _player->OnHit.AddRaw(this, &MultihitIdleRelic::OnHit);
}

void MultihitIdleRelic::UnBind(ADwarfPlayerState* _player, Cave* _cave)
{
	handle.Reset();
}
/////////////
/// Drill ///
/////////////
void DrillIdleRelic::Bind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg = _player->Drill.OnDamageCalc.AddRaw(this, &DrillIdleRelic::DamageCalc);
	handleCd = _player->Drill.OnCooldownCalc.AddRaw(this, &DrillIdleRelic::CooldownCalc);
}
void DrillIdleRelic::UnBind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg.Reset();
	handleCd.Reset();
}	
void DrillIdleRelic::DamageCalc(BigNumber& _damage)
{
	_damage *= damageMultiplier;
}
void DrillIdleRelic::CooldownCalc(float& _cd)
{
	_cd *= cooldownMultiplier;
}

int DrillIdleRelic::GetDamageMult(int _level)
{
	switch (_level)
	{
	case 1: return 5;
	case 2: return 40;
	case 3: return 250;
	case 4: return 1500;
	case 5: return 10000;
	}
	return 1;
}

float DrillIdleRelic::GetCooldownMult(int _level)
{
	switch (_level)
	{
	case 1: return 0.75f;
	case 2: return 0.5f;
	case 3: return 0.3f;
	case 4: return 0.2f;
	case 5: return 0.1f;
	}
	return 1;
}

void DrillIdleRelic::RankUp()
{
	IdleRelic::RankUp();
	damageMultiplier = GetDamageMult(rank);
	cooldownMultiplier = GetCooldownMult(rank);
}

FString DrillIdleRelic::GetDescriptionText(int _level)
{
	return "Increases your Drill's damage by +" + FString::FromInt((GetDamageMult(_level)-1) * 100) + "% and it's attack speed by +" + FString::FromInt((1.0f / GetCooldownMult(_level) - 1) * 100) + "%.";
}

////////////
/// Boom ///
////////////
void BoomIdleRelic::Bind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg = _player->Boom.OnDamageCalc.AddRaw(this, &BoomIdleRelic::DamageCalc);
	handleCd = _player->Boom.OnCooldownCalc.AddRaw(this, &BoomIdleRelic::CooldownCalc);
}
void BoomIdleRelic::UnBind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg.Reset();
	handleCd.Reset();
}
void BoomIdleRelic::DamageCalc(BigNumber& _damage)
{
	_damage *= damageMultiplier;
}
void BoomIdleRelic::CooldownCalc(float& _cd)
{
	_cd *= cooldownMultiplier;
}

int BoomIdleRelic::GetDamageMult(int _level)
{
	switch (_level)
	{
	case 1: return 5;
	case 2: return 40;
	case 3: return 250;
	case 4: return 1500;
	case 5: return 10000;
	}
	return 1;
}

float BoomIdleRelic::GetCooldownMult(int _level)
{
	switch (_level)
	{
	case 1: return 0.75f;
	case 2: return 0.5f;
	case 3: return 0.3f;
	case 4: return 0.2f;
	case 5: return 0.1f;
	}
	return 1;
}

void BoomIdleRelic::RankUp()
{
	IdleRelic::RankUp();
	damageMultiplier = GetDamageMult(rank);
	cooldownMultiplier = GetCooldownMult(rank);
}

FString BoomIdleRelic::GetDescriptionText(int _level)
{
	return "Increases your TNT's damage by +" + FString::FromInt((GetDamageMult(_level)-1) * 100) + "% and it's attack speed by +" + FString::FromInt((1.0f / GetCooldownMult(_level) - 1) * 100) + "%.";
}

//////////////////
/// Earthquake ///
//////////////////
void EarthquakeIdleRelic::Bind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg = _player->Earthquake.OnDamageCalc.AddRaw(this, &EarthquakeIdleRelic::DamageCalc);
	handleCd = _player->Earthquake.OnCooldownCalc.AddRaw(this, &EarthquakeIdleRelic::CooldownCalc);
}
void EarthquakeIdleRelic::UnBind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg.Reset();
	handleCd.Reset();
}
void EarthquakeIdleRelic::DamageCalc(BigNumber& _damage)
{
	_damage *= damageMultiplier;
}
void EarthquakeIdleRelic::CooldownCalc(float& _cd)
{
	_cd *= cooldownMultiplier;
}

int EarthquakeIdleRelic::GetDamageMult(int _level)
{
	switch (_level)
	{
	case 1: return 5;
	case 2: return 40;
	case 3: return 250;
	case 4: return 1500;
	case 5: return 10000;
	}
	return 1;
}

float EarthquakeIdleRelic::GetCooldownMult(int _level)
{
	switch (_level)
	{
	case 1: return 0.75f;
	case 2: return 0.5f;
	case 3: return 0.3f;
	case 4: return 0.2f;
	case 5: return 0.1f;
	}
	return 1;
}

void EarthquakeIdleRelic::RankUp()
{
	IdleRelic::RankUp();
	damageMultiplier = GetDamageMult(rank);
	cooldownMultiplier = GetCooldownMult(rank);
}

FString EarthquakeIdleRelic::GetDescriptionText(int _level)
{
	return "Increases your Earthquake Totem's damage by +" + FString::FromInt((GetDamageMult(_level) - 1) * 100) + "% and it's attack speed by +" + FString::FromInt((1.0f / GetCooldownMult(_level) - 1) * 100) + "%.";
}

/////////////
/// Laser ///
/////////////
void LaserIdleRelic::Bind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg = _player->Laser.OnDamageCalc.AddRaw(this, &LaserIdleRelic::DamageCalc);
	handleCd = _player->Laser.OnCooldownCalc.AddRaw(this, &LaserIdleRelic::CooldownCalc);
}
void LaserIdleRelic::UnBind(ADwarfPlayerState* _player, Cave* _cave)
{
	handleDmg.Reset();
	handleCd.Reset();
}
void LaserIdleRelic::DamageCalc(BigNumber& _damage)
{
	_damage *= damageMultiplier;
}
void LaserIdleRelic::CooldownCalc(float& _cd)
{
	_cd *= cooldownMultiplier;
}

int LaserIdleRelic::GetDamageMult(int _level)
{
	switch (_level)
	{
	case 1: return 5;
	case 2: return 40;
	case 3: return 250;
	case 4: return 1500;
	case 5: return 10000;
	}
	return 1;
}

float LaserIdleRelic::GetCooldownMult(int _level)
{
	switch (_level)
	{
	case 1: return 0.75f;
	case 2: return 0.5f;
	case 3: return 0.3f;
	case 4: return 0.2f;
	case 5: return 0.1f;
	}
	return 1;
}

void LaserIdleRelic::RankUp()
{
	IdleRelic::RankUp();
	damageMultiplier = GetDamageMult(rank);
	cooldownMultiplier = GetCooldownMult(rank);
}

FString LaserIdleRelic::GetDescriptionText(int _level)
{
	return "Increases your Mining Laser's damage by +" + FString::FromInt((GetDamageMult(_level) - 1) * 100) + "% and it's attack speed by +" + FString::FromInt((1.0f / GetCooldownMult(_level) - 1) * 100.0f) + "%.";
}