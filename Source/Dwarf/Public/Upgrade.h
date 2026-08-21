#pragma once

#include "CoreMinimal.h"
#include "ResourceData.h"
#include "UpgradeEntryWidget.h"
//#include "UpgradeType.h"

class ADwarfPlayerState;

enum UpgradeType : int
{
	UPG_STRONG_ARMS = 0,	// Click damage
	UPG_DRILL,				// Auto dmg
	UPG_BOOM,				// Auto dmg column
	UPG_EARTHQUAKE,			// Auto dmg area
	UPG_LASER,				// Auto dmg row
	UPG_PRECISION,			// Click crit chance
	UPG_GOLD_LOVER,			// Global Yield
	UPG_PIERCE,

	UPGRADE_COUNT
};

struct ResourceUpgrade
{
	ResourceUpgrade() {};
	virtual ~ResourceUpgrade() {};

	ADwarfPlayerState* player;

	UPROPERTY()
	FString upgradeFunctionName;

	UPROPERTY()
	FString displayName;

	UPROPERTY()
	unsigned int upgradeLevel = 0;

	UPROPERTY()
	TArray<ResourceData> costCached;

	// Texture/Image
	UPROPERTY()
	UTexture2D* icon;

	int buyMultiplier = 1;
	void SetBuyMultiplier(int _mult);

	UUpgradeEntryWidget* widget;

	void SetWidget(UUpgradeEntryWidget* _widget);

	void LevelUpgrade();

	virtual TArray<ResourceData> GetCost(int _count) = 0;
	virtual FString GetDescription() = 0;
	virtual void ApplyUpgrade() = 0;

	TArray<ResourceData> GetCostAndCache();
	void UpdateTooltipText();

};

struct StrongArmsUpgrade: public ResourceUpgrade
{
	StrongArmsUpgrade() {
		displayName = "Strong Arms";
		upgradeFunctionName = "StrongArms";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/StrongArms.StrongArms"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetBaseDamage(int _level);

};

struct DrillUpgrade: public ResourceUpgrade
{
	DrillUpgrade() {
		displayName = "Drill";
		upgradeFunctionName = "Drill";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Drill.Drill"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetBaseDamage(int _level);
	float GetCooldown(int _level);
};
struct BoomUpgrade: public ResourceUpgrade
{
	BoomUpgrade() {
		displayName = "TNT";
		upgradeFunctionName = "Boom";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/TNT.TNT"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetBaseDamage(int _level);
	float GetCooldown(int _level);

};
struct EarthquakeUpgrade: public ResourceUpgrade
{
	EarthquakeUpgrade() {
		displayName = "Earthquake Totem";
		upgradeFunctionName = "Earthquake";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Totem.Totem"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetBaseDamage(int _level);
	float GetCooldown(int _level);

};
struct LaserUpgrade: public ResourceUpgrade
{
	LaserUpgrade() {
		displayName = "Mining Laser";
		upgradeFunctionName = "Laser";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Laser.Laser"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetBaseDamage(int _level);
	float GetCooldown(int _level);

};
struct CritUpgrade: public ResourceUpgrade
{
	CritUpgrade() {
		displayName = "Precision";
		upgradeFunctionName = "Crit";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Crit.Crit"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetMult(int _level);

};
struct YieldUpgrade: public ResourceUpgrade
{
	YieldUpgrade() {
		displayName = "Gold Lover";
		upgradeFunctionName = "Yield";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Coin.Coin"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetMult(int _level);

};
struct PlaceholderUpgrade: public ResourceUpgrade
{
	PlaceholderUpgrade() {
		displayName = "Placeholder";
		upgradeFunctionName = "Placeholder";
		icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Character/1454055200561172541.1454055200561172541"));
	}

	virtual TArray<ResourceData> GetCost(int _level);
	virtual FString GetDescription();
	virtual void ApplyUpgrade();
	BigNumber GetMult(int _level);

};