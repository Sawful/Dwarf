// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleRelic.h"
#include "DwarfPlayerState.h"

IdleRelic::IdleRelic() { countRequired = GetRequiredCount(); }

void DamageIdleRelic::Bind(ADwarfPlayerState* _player)
{
	handle = _player->OnClickDamageCalc.AddRaw(this, &DamageIdleRelic::DamageCalc);
}

void DamageIdleRelic::UnBind(ADwarfPlayerState* _player)
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

void DamageIdleRelic::DamageCalc(int& _damage)
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

void MultihitIdleRelic::Bind(ADwarfPlayerState* _player)
{
	handle = _player->OnHit.AddRaw(this, &MultihitIdleRelic::OnHit);
}

void MultihitIdleRelic::UnBind(ADwarfPlayerState* _player)
{
	handle.Reset();
}

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
