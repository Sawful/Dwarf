#include "Upgrade.h"
#include "Components/RichTextBlock.h"
#include "MathTools.h"
#include "DwarfPlayerState.h"

TArray<ResourceData> ResourceUpgrade::GetCostAndCache()
{
	costCached.Empty(costCached.Num());
	costCached = GetCost(upgradeLevel);
	int typeCount = costCached.Num();
	for (int i = 1; i < buyMultiplier; i++)
	{
		TArray<ResourceData> currentCost = GetCost(upgradeLevel + i);
		for (int j = 0; j < typeCount; j++)
		{
			costCached[j].Amount += currentCost[j].Amount;
		}
	}
	
	return costCached;
}

void ResourceUpgrade::UpdateTooltipText()
{
	widget->level = upgradeLevel;
	widget->LevelText->SetText(FText::FromString(FString::FromInt(upgradeLevel)));
	widget->name = displayName;
	widget->descriptionText = GetDescription();
}

void ResourceUpgrade::SetBuyMultiplier(int _mult)
{
	buyMultiplier = _mult;
	UpdateTooltipText();
	GetCostAndCache();
}

void ResourceUpgrade::SetWidget(UUpgradeEntryWidget* _widget)
{
	widget = _widget;
	widget->Icon->SetBrushFromTexture(icon);
}

void ResourceUpgrade::LevelUpgrade()
{
	for (int i = 0; i < buyMultiplier; i++)
	{
		upgradeLevel++;
		ApplyUpgrade();
	}
}

///////////////////
/// Strong Arms ///
///////////////////

TArray<ResourceData> StrongArmsUpgrade::GetCost(int _level)
{
	const uint64 baseValueMudrock = 2;
	const float s1 = 0.4f;
	const float s2 = 20;
	TArray<ResourceData> cost;
	cost.Add({ MUDROCK, MagicFunction(_level, baseValueMudrock, s1, s2) });
	return cost;
}

FString StrongArmsUpgrade::GetDescription()
{
	return "Your clicks deal " + GetDamage(upgradeLevel).ToStringTrunc() + "->" + GetDamage(upgradeLevel + buyMultiplier).ToStringTrunc() + " damage.";
}

void StrongArmsUpgrade::ApplyUpgrade()
{
	player->MinDamage = GetDamage(upgradeLevel);
}

BigNumber StrongArmsUpgrade::GetDamage(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 2, 0.4, 5);
}

/////////////
/// DRILL ///
/////////////

TArray<ResourceData> DrillUpgrade::GetCost(int _level)
{
	const uint64 baseValue = 3;
	const float s1 = 0.4f;
	const float s2 = 15;
	TArray<ResourceData> cost;
	cost.Add({ COAL, MagicFunction(_level, baseValue, s1, s2) });
	cost.Add({ COPPER, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

FString DrillUpgrade::GetDescription()
{
	return "An automatic drill that will hit for " + GetDamage(upgradeLevel).ToStringTrunc() + "->" + GetDamage(upgradeLevel + buyMultiplier).ToStringTrunc() + " for you.";
}

void DrillUpgrade::ApplyUpgrade()
{
	AutomaticDamager* drill = &player->Drill;
	drill->Damage = GetDamage(upgradeLevel);
	switch (upgradeLevel)
	{
	case 1:
		drill->SetDamagerActive(true);
		break;
	case 10:
		drill->MilestoneDamageMult *= 2;
		drill->Downtime *= 0.8;
		break;
	case 25:
		drill->MilestoneDamageMult *= 2.5f;
		drill->Downtime *= 0.8;
		break;
	case 50:
		drill->MilestoneDamageMult *= 3;
		drill->Downtime *= 0.8;
		break;
	case 100:
		drill->MilestoneDamageMult *= 3.5f;
		drill->Downtime *= 0.8;
		break;
	case 250:
		drill->MilestoneDamageMult *= 4;
		drill->Downtime *= 0.8;
		break;
	case 500:
		drill->MilestoneDamageMult *= 5;
		drill->Downtime *= 0.8;
		break;
	}

	drill->UpdateDisplayTooltip(upgradeLevel);
}

BigNumber DrillUpgrade::GetDamage(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 10, 0.2, 10) * player->Drill.MilestoneDamageMult;
}

////////////
/// BOOM ///
////////////

TArray<ResourceData> BoomUpgrade::GetCost(int _level)
{
	const uint64 baseValue = 25;
	const float s1 = 0.45f;
	const float s2 = 14;
	TArray<ResourceData> cost;
	cost.Add({ SULFUR, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

FString BoomUpgrade::GetDescription()
{
	return "A slow automatic hit that deals heavy damage (" + GetDamage(upgradeLevel).ToStringTrunc() + "->" + GetDamage(upgradeLevel + buyMultiplier).ToStringTrunc() + ") to the first column of blocks.";
}

void BoomUpgrade::ApplyUpgrade()
{
	AutomaticDamager& Boom = player->Boom;
	Boom.Damage = GetDamage(upgradeLevel);
		
	switch (upgradeLevel)
	{
	case 1:
		Boom.SetDamagerActive(true);
		break;
	case 10:
		Boom.MilestoneDamageMult *= 2;
		Boom.Downtime *= 0.9;
		break;
	case 25:
		Boom.MilestoneDamageMult *= 3;
		Boom.Downtime *= 0.9;
		break;
	case 50:
		Boom.MilestoneDamageMult *= 4;
		Boom.Downtime *= 0.9;
		break;
	case 100:
		Boom.MilestoneDamageMult *= 5;
		Boom.Downtime *= 0.9;
		break;
	case 250:
		Boom.MilestoneDamageMult *= 6;
		Boom.Downtime *= 0.9;
		break;
	case 500:
		Boom.MilestoneDamageMult *= 7;
		Boom.Downtime *= 0.9;
		break;
	}

	Boom.UpdateDisplayTooltip(upgradeLevel);
}

BigNumber BoomUpgrade::GetDamage(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 50, 0.25, 10) * player->Boom.MilestoneDamageMult;
}

//////////////////
/// EARTHQUAKE ///
//////////////////

TArray<ResourceData> EarthquakeUpgrade::GetCost(int _level)
{
	const uint64 baseValue = 15;
	const float s1 = 0.45f;
	const float s2 = 15;
	TArray<ResourceData> cost;
	cost.Add({ IRON, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

FString EarthquakeUpgrade::GetDescription()
{
	return "Shakes up the ground in front of you for " + GetDamage(upgradeLevel).ToStringTrunc() + "->" + GetDamage(upgradeLevel + buyMultiplier).ToStringTrunc() + " to clear any rubble standing in your way.";
}

void EarthquakeUpgrade::ApplyUpgrade()
{
	AutomaticDamager& EarthquakeTotem = player->Earthquake;
	EarthquakeTotem.Damage = GetDamage(upgradeLevel);
	switch (upgradeLevel)
	{
	case 1:
		EarthquakeTotem.SetDamagerActive(true);
		break;
	case 10:
		EarthquakeTotem.MilestoneDamageMult *= 2;
		EarthquakeTotem.Downtime *= 0.7;
		break;
	case 25:
		EarthquakeTotem.MilestoneDamageMult *= 2;
		EarthquakeTotem.Downtime *= 0.7;
		break;
	case 50:
		EarthquakeTotem.MilestoneDamageMult *= 2;
		EarthquakeTotem.Downtime *= 0.7;
		break;
	case 100:
		EarthquakeTotem.MilestoneDamageMult *= 2;
		EarthquakeTotem.Downtime *= 0.7;
		break;
	case 250:
		EarthquakeTotem.MilestoneDamageMult *= 2;
		EarthquakeTotem.Downtime *= 0.7;
		break;
	case 500:
		EarthquakeTotem.MilestoneDamageMult *= 2;
		EarthquakeTotem.Downtime *= 0.7;
		break;
	}

	EarthquakeTotem.UpdateDisplayTooltip(upgradeLevel);
}

BigNumber EarthquakeUpgrade::GetDamage(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 10, 0.3, 10) * player->Earthquake.MilestoneDamageMult;
}		    

/////////////
/// LASER ///
/////////////
TArray<ResourceData> LaserUpgrade::GetCost(int _level)
{
	const uint64 baseValue = 5;
	const float s1 = 0.45f;
	const float s2 = 14;
	TArray<ResourceData> cost;
	cost.Add({ DIAMOND, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

FString LaserUpgrade::GetDescription()
{
	return "Constantly piercing through in front of it for " + GetDamage(upgradeLevel).ToStringTrunc() + "->" + GetDamage(upgradeLevel + buyMultiplier).ToStringTrunc() + ".";
}

void LaserUpgrade::ApplyUpgrade()
{
	AutomaticDamager& Laser = player->Laser;
	Laser.Damage = GetDamage(upgradeLevel);
	switch (upgradeLevel)
	{
	case 1:
		Laser.SetDamagerActive(true);
		break;
	case 10:
		Laser.MilestoneDamageMult *= 2;
		Laser.Downtime *= 0.9;
		break;
	case 25:
		Laser.MilestoneDamageMult *= 3;
		Laser.Downtime *= 0.9;
		break;
	case 50:
		Laser.MilestoneDamageMult *= 4;
		Laser.Downtime *= 0.9;
		break;
	case 100:
		Laser.MilestoneDamageMult *= 5;
		Laser.Downtime *= 0.9;
		break;
	case 250:
		Laser.MilestoneDamageMult *= 6;
		Laser.Downtime *= 0.9;
		break;
	case 500:
		Laser.MilestoneDamageMult *= 7;
		Laser.Downtime *= 0.9;
		break;
	}

	Laser.UpdateDisplayTooltip(upgradeLevel);
}

BigNumber LaserUpgrade::GetDamage(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 5, 0.4, 10) * player->Laser.MilestoneDamageMult;
}

////////////
/// Crit ///
////////////

TArray<ResourceData> CritUpgrade::GetCost(int _level)
{
	const uint64 baseValue = 20;
	const float s1 = 0.40f;
	const float s2 = 15;
	TArray<ResourceData> cost;
	cost.Add({ TIN, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

FString CritUpgrade::GetDescription()
{
	return "Clicks can be critical hits, doing " + GetMult(upgradeLevel).ToString() + "->" + GetMult(upgradeLevel + buyMultiplier).ToString() + " times the base damage.";
}

void CritUpgrade::ApplyUpgrade()
{
	player->CritMultiplier = GetMult(upgradeLevel);
	switch (upgradeLevel)
	{
	case 1:
		player->CritChance = 5;
		break;
	}
}

BigNumber CritUpgrade::GetMult(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 2, 0.2, 10);
}

/////////////
/// Yield ///
/////////////

TArray<ResourceData> YieldUpgrade::GetCost(int _level)
{
	const uint64 baseValue = 10;
	const float s1 = 0.40f;
	const float s2 = 10;
	TArray<ResourceData> cost;
	cost.Add({ SILVER, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

FString YieldUpgrade::GetDescription()
{
	return "Multiply your resource yield by " + GetMult(upgradeLevel).ToString() + "->" + GetMult(upgradeLevel + buyMultiplier).ToString() + ".";
}

void YieldUpgrade::ApplyUpgrade()
{
	player->GlobalYieldMultiplier = GetMult(upgradeLevel);
}

BigNumber YieldUpgrade::GetMult(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 0.1f, 0.4f, 5) + 1;
}

///////////////////
/// Placeholder ///
///////////////////

TArray<ResourceData> PlaceholderUpgrade::GetCost(int _level)
{
	const uint64 baseValue = 9999999;
	const float s1 = 0.40f;
	const float s2 = 10;
	TArray<ResourceData> cost;
	cost.Add({ OBSIDIAN, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

FString PlaceholderUpgrade::GetDescription()
{
	return "Does nothing. This will later be a \"Pierce\" upgrade, dealing a percent of your click damage to the X next blocks.";
}

void PlaceholderUpgrade::ApplyUpgrade()
{
}

BigNumber PlaceholderUpgrade::GetMult(int _level)
{
	if (_level <= 0) return 0;
	return MagicFunction(_level - 1, 0.1f, 0.4f, 5) + 1;
}