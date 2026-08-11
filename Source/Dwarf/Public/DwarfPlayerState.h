// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/TextRenderActor.h"

#include "DwarfCameraActor.h"

#include "MainMenuWidget.h"
#include "IdleHUD.h"
#include "CharacterMenuWidget.h"
#include "RogueHUD.h"
#include "RogueCardSelection.h"

#include "RogueItem.h"
#include "IdleRelic.h"
#include "Upgrade.h"
#include "Block.h"
#include "Cave.h"
#include "DamageSource.h"
#include "DwarfPawn.h"
#include "DwarfPlayerState.generated.h"

class USaveGame;
class UUpgradeEntryData;

TArray<ResourceData> CostStrongArms(unsigned int _level);
TArray<ResourceData> CostDrill(unsigned int _level);
TArray<ResourceData> CostBoom(unsigned int _level);
TArray<ResourceData> CostLaser(unsigned int _level);
TArray<ResourceData> CostEarthquake(unsigned int _level);

DECLARE_DELEGATE_OneParam(FOnMilestoneTier, int);
template <typename T>
struct Milestone
{
	T value; // Tracked value
	int currentTier = 0;
	int maximumTier;
	TArray<T> tiers;
	FOnMilestoneTier tierUpDelegate;

	Milestone(T _value) : value(_value) {};

	void CheckTier();
};

struct SavedStats
{
	Milestone<int> damageDone = 0;
	Milestone<int> blocksBroken = 0;
	Milestone<int> metersWalked = 0;
	Milestone<int> rebirthCount = 0;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageCalc, int&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnExpCalc, int&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCooldownCalc, float&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHit, ADwarfPlayerState*);

enum DamageType
{
	FIRST,
	COLUMN,
	LINE,
	AREA
};
struct AutomaticDamager
{
	int Damage = 0;
	float MilestoneDamageMult = 1;
	float Downtime = 2;
	float Clock;
	bool active = false;
	DamageSource source;
	DamageType damageType;
	UAutoAttackerDisplay* Display;
	FOnDamageCalc OnDamageCalc;
	FOnCooldownCalc OnCooldownCalc;
	bool IsHitting();
	float GetDPS() { return (float)Damage / Downtime; };
	void SetDamagerActive(bool _active);
	void UpdateDisplayTooltip(int _level);
};


struct RoguePlayerData
{
	int level = 1;
	int experience = 0;
	int experienceRequired = 8;
	TArray<RogueItem*> items;
	TArray<UUpgradeEntryWidget*> itemWidgets;

	bool timePaused = false;

	float clickCooldown = 1.0f;
	float clickTimer = 1.0f;

	DamageSource ClickSource;
	int MinDamage = 10;
	int MaxDamage = 15;
	float MaxPressure = 100;
	float Pressure = 100;
	float PressureResistance = 0; // 0 <=> full damage, 1 <=> invulnerable
	FOnDamageCalc OnDamageCalc;
	FOnExpCalc OnExpCalc;
	FOnCooldownCalc OnCooldownCalc;
	void LevelUpRogue();
	float GetHitCooldown();
};

UCLASS()
class DWARF_API ADwarfPlayerState : public APlayerState
{
	GENERATED_BODY()
	ADwarfPlayerState();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void SetupResourceUpgrades();
	void SetupResourceUpgradeDelegate(ResourceUpgrade& upgrade, TArray<ResourceData>(* InFunc)(unsigned int));
	void SetupMilestones();
	void InitializeAutomaticDamagers();
	void InitRelicsArray();
	TArray<IdleRelic*> relics;

	bool gameLoaded = false;

	UTexture2D* StrongArmsIcon;
	UTexture2D* PrecisionIcon;
	UTexture2D* GoldLoverIcon;
	UTexture2D* PlaceholderIcon;
	UTexture2D* DrillIcon;
	UTexture2D* TNTIcon;
	UTexture2D* EarthquakeIcon;
	UTexture2D* LaserIcon;

	// Main camera (follows the dwarf)
	UPROPERTY(EditAnywhere)
	ADwarfCameraActor* CameraActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf <ATextRenderActor> DamageTextClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuWidget> MenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UIdleHUD> HUDClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterMenuWidget> CharacterMenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> TooltipClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URogueHUD> RogueHUDClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUpgradeEntryWidget> UpgradeBoxClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUpgradeEntryWidget> ItemBoxClass;

	UMainMenuWidget* MainMenu; 
	UIdleHUD* HUD;
	UCharacterMenuWidget* CharacterMenu;
	URogueHUD* RogueHUD;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URogueCardSelection> CardSelectionClass;
	URogueCardSelection* CardSelection;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URogueCardSelection> RelicSelectionClass;
	URogueCardSelection* RelicSelection;
	IdleRelic* relicCardSelection[3];

	ResourceUpgrade resourceUpgrades[UPGRADE_COUNT];

	TArray<IdleRelic*> obtainedRelics;

	// ROGUE //
	bool inRun = false;
	RoguePlayerData rogueData;
	TArray<RogueItem*> unlockedItemPool;
	TArray<RogueItem*> currentItemPool;
	int itemsSelection[3];

	bool TimeBoost = false;
	float TimeBoostFactor = 5.0f;
	float TimeBoostLeft = 10.0f;

public:
	void StartGame();

	void OnSaveFinished(const FString& _name, const int32 _userIndex, bool _success);
	void OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	
	void ResetDwarfStats();

	UFUNCTION()
	void SaveCurrentStateAsync();
	void SaveCurrentState();

	UFUNCTION()
	void Rebirth();

	bool CheckCost(const TArray<ResourceData>& _cost);
	bool PayCost(const TArray<ResourceData>& _cost);
	void BuyResourceUpgrade(UpgradeType _upgrade);
	void ApplyResourceUpgrade(UpgradeType _upgrade, unsigned int _level);
	FString CreateCostText(UpgradeType _upgrade, const TArray<ResourceData>& _cost);
	void RebuildCostCache(UpgradeType _upgrade);
	int GetMaxUpgradeMult(UpgradeType _upgrade);

	UFUNCTION()
	void CardSelectLeft();
	UFUNCTION()
	void CardSelectMiddle();
	UFUNCTION()
	void CardSelectRight();

	void AddItem(RogueItem* _item);
	void AddItemFromPool(unsigned int _index);

	UFUNCTION()
	void RelicCardSelectLeft();
	UFUNCTION()
	void RelicCardSelectMiddle();
	UFUNCTION()
	void RelicCardSelectRight();
	void AddRelic(IdleRelic* _relic);
	void AddRelic(IdleRelic* _relic, int _count);

	bool resourcesDirty = false;

	UFUNCTION()
	void FocusIdle();

	UFUNCTION()
	void FocusRogue();

	UFUNCTION()
	void FocusMenu();
	
	UFUNCTION()
	void StartRun();

	UFUNCTION()
	void RunRewards();

	UFUNCTION()
	void EndRun();

	UFUNCTION()
	void ShowCharacterMenu();

	UFUNCTION()
	void HideCharacterMenu();

	UFUNCTION()
	void UpgradeStrongArms();
	UFUNCTION()
	void UpgradeCrit();
	UFUNCTION()
	void UpgradeYield();
	UFUNCTION()
	void UpgradeDrill();
	UFUNCTION()
	void UpgradeBoom();
	UFUNCTION()
	void UpgradeLaser();
	UFUNCTION()
	void UpgradeEarthquake();

	bool maxMultiplier = false;
	int upgradeMultiplier[UPGRADE_COUNT];
	UFUNCTION()
	void SetUpgradeMult1();
	UFUNCTION()
	void SetUpgradeMult5();
	UFUNCTION()
	void SetUpgradeMult10();
	UFUNCTION()
	void SetUpgradeMult25();
	UFUNCTION()
	void SetUpgradeMult100();
	UFUNCTION()
	void SetUpgradeMultMax();

	UFUNCTION()
	void BlockMilestone(int _tier);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ADwarfPawn> DwarfPawnClass;

	ADwarfPawn* idlePawn;
	Cave* currentCave = nullptr;
	Cave idleCave;
	int difficulty = 1;
	UFUNCTION()
	void ChangeDifficulty(float _value);

	// Dwarf Stats
	int Level;
	int Experience;
	int RequiredExperience;
	void IncreaseExp(int _value);
	void LevelUp();
	void LevelUpRogue();
	void IncreaseRogueExp(int _value);
	int TalentPoints;

	float GlobalYieldMultiplier = 1.0f;
	float resourceYieldMultiplier[RESOURCE_COUNT];
	float blockYieldMultiplier[BLOCK_COUNT];

	FString GetUpgradeDamageText(UpgradeType _upgrade);

	DamageSource ClickSource;
	int MinDamage = 1;
	int MaxDamage = 1; // MaxDamage is MinDamage * DamageWindow
	float CritMultiplier;
	int CritChance;
	float DamageWindow = 1.0f;
	float ClickDamageMultiplier = 1.0f;
	int GetClickDamage();
	int GetRogueClickDamage();

	AutomaticDamager Drill;
	AutomaticDamager Boom;
	AutomaticDamager Laser;
	AutomaticDamager EarthquakeTotem;

	void UpdateDamager(AutomaticDamager& _damager, float _dt);

	void Hit(bool _silent);
	void Damage(int _damage, DamageSource _source, Cave* _cave);
	void DamageColumn(int _damage, DamageSource _source, Cave* _cave);
	void DamageIdleCave(int _damage, DamageSource _source);

	void BlockRewardIdle(BlockData _data);
	void BlockRewardRogue(BlockData _data);
	int GetRogueExp(int _exp);

	FOnDamageCalc OnClickDamageCalc;
	FOnCooldownCalc OnCooldownCalc;
	FOnHit OnHit;

	// Stored stats
	SavedStats savedStats;
	
	void IncreaseBlocks();
	void IncreaseWalk();

	bool isResourceUnlocked[RESOURCE_COUNT];
	BigNumber resources[RESOURCE_COUNT];
};

template<typename T>
inline void Milestone<T>::CheckTier()
{
	while (currentTier < maximumTier && value > tiers[currentTier])
	{
		// Call milestone function
		tierUpDelegate.Execute(currentTier);
		currentTier++;
	}
}
