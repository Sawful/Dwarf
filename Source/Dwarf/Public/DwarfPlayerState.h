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

#include "RogueItem.h"
#include "IdleRelic.h"
#include "Upgrade.h"
#include "Block.h"
#include "Cave.h"
#include "DamageSource.h"
#include "DwarfPawn.h"
#include "Milestone.h"
#include "RoguePlayerData.h"
#include "DwarfPlayerState.generated.h"

class USaveGame;
class UUpgradeEntryData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHit, ADwarfPlayerState*);

struct SavedStats
{
	Milestone<int> damageDone = 0;
	Milestone<int> blocksBroken = 0;
	Milestone<int> metersWalked = 0;
	Milestone<int> rebirthCount = 0;
};

enum DamageType
{
	FIRST,
	COLUMN,
	LINE,
	AREA
};
struct AutomaticDamager
{
	BigNumber Damage = 0;
	BigNumber MilestoneDamageMult = 1;
	float Downtime = 2;
	float Clock;
	bool active = false;
	DamageSource source;
	DamageType damageType;
	UAutoAttackerDisplay* Display;
	FOnDamageCalc OnDamageCalc;
	FOnCooldownCalc OnCooldownCalc;

	BigNumber GetDamage();
	float GetCooldown();

	bool IsHitting();
	BigNumber GetDPS() { return Damage / (BigNumber)Downtime; };
	void SetDamagerActive(bool _active);
	void UpdateDisplayTooltip(int _level);
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
	void SetupMilestones();
	void InitializeAutomaticDamagers();
	void InitRelicsArray();
	TArray<IdleRelic*> relics;

	bool gameLoaded = false;

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

	UPROPERTY()
	UMainMenuWidget* MainMenu;
	UPROPERTY()
	UIdleHUD* HUD;
	UPROPERTY()
	UCharacterMenuWidget* CharacterMenu;
	UPROPERTY()
	URogueHUD* RogueHUD;

	ResourceUpgrade* resourceUpgrades[UpgradeType::UPGRADE_COUNT];

	TArray<IdleRelic*> obtainedRelics;

	// ROGUE //
	bool inRun = false;
	URoguePlayerData* rogueData;
	UPROPERTY()
	TArray<URogueItem*> unlockedItemPool;
	IdleRelic* relicCardSelection[3];

	bool TimeBoost = false;
	float TimeBoostFactor;
	float TimeBoostLeft;

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

	UFUNCTION(BlueprintCallable)
	void BuyResourceUpgrade(UpgradeType _upgrade);

	FString CreateCostText(UpgradeType _upgrade, const TArray<ResourceData>& _cost);
	void RebuildCostCache(UpgradeType _upgrade);
	int GetMaxUpgradeMult(UpgradeType _upgrade);

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
	void QuitGame();
	
	UFUNCTION()
	void StartRun();
	void BuildItemPool();

	UFUNCTION()
	void RunRewards();

	UFUNCTION()
	void EndRun();

	UFUNCTION()
	void ShowCharacterMenu();

	UFUNCTION()
	void HideCharacterMenu();

	bool maxMultiplier = false;

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
	int Level = 0;
	int Experience = 0;
	int RequiredExperience;
	void IncreaseExp(int _value);
	void LevelUp();
	int TalentPoints;

	BigNumber GlobalYieldMultiplier = 1.0f;
	BigNumber resourceYieldMultiplier[RESOURCE_COUNT];
	BigNumber blockYieldMultiplier[BLOCK_COUNT];

	FString GetUpgradeDamageText(UpgradeType _upgrade);

	DamageSource ClickSource;
	BigNumber MinDamage = 1;
	BigNumber MaxDamage = 1; // MaxDamage is MinDamage * DamageWindow
	BigNumber CritMultiplier;
	BigNumber CritChance;
	BigNumber DamageWindow = 1.0f;
	BigNumber ClickDamageMultiplier = 1.0f;
	BigNumber GetClickDamage();

	AutomaticDamager Drill;
	AutomaticDamager Boom;
	AutomaticDamager Laser;
	AutomaticDamager Earthquake;

	void UpdateDamager(AutomaticDamager& _damager, float _dt);

	void Hit(bool _silent);
	void Damage(BigNumber _damage, DamageSource _source, Cave* _cave);
	void DamageColumn(BigNumber _damage, DamageSource _source, Cave* _cave);
	void DamageArea(BigNumber _damage, DamageSource _source, Cave* _cave);
	void DamageRow(BigNumber _damage, DamageSource _source, Cave* _cave);
	void DamageIdleCave(BigNumber _damage, DamageSource _source);

	void BlockRewardIdle(BlockData _data);

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