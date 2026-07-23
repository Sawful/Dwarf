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
#include "Upgrade.h"
#include "Block.h"
#include "Cave.h"
#include "DwarfPawn.h"
#include "DwarfPlayerState.generated.h"

class USaveGame;
class UUpgradeEntryData;

TArray<ResourceData> CostStrongArms(int _level);
TArray<ResourceData> CostDrill(int _level);
TArray<ResourceData> CostBoom(int _level);

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
	UPROPERTY()
	Milestone<int> damageDone = 0;
	UPROPERTY()
	Milestone<int> blocksBroken = 0;
	UPROPERTY()
	Milestone<int> metersWalked = 0;
	UPROPERTY()
	Milestone<int> rebirthCount = 0;
};

enum DamageTextType
{
	NORMAL,
	AUTO,
	CRITICAL
};

struct DamageSource
{
	float BlockDamageMultiplier[BLOCK_COUNT];
	DamageTextType TextType;
	DamageSource() { std::fill_n(BlockDamageMultiplier, BLOCK_COUNT, 1.0f); };
};

struct AutomaticDamager
{
	int Damage = 0;
	float Downtime = 2;
	float Clock;
	bool active = false;
	DamageSource source;
	UAutoAttackerDisplay* Display;
	bool IsHitting();
	float GetDPS() { return (float)Damage / Downtime; };
	void SetDamagerActive(bool _active);
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageCalc, int&);
struct RoguePlayerData
{
	int level = 1;
	int experience = 0;
	int experienceRequired = 8;
	TArray<RogueItem*> items;

	DamageSource ClickSource;
	int MinDamage = 10;
	int MaxDamage = 15;
	FOnDamageCalc OnDamageCalc;
	void LevelUpRogue();
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
	void SetupResourceUpgradeDelegate(ResourceUpgrade& upgrade, TArray<ResourceData>(* InFunc)(int));
	void SetupMilestones();
	void InitializeAutomaticDamagers();

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
	TSubclassOf<URogueHUD> RogueHUDClass;

	UMainMenuWidget* MainMenu; 
	UIdleHUD* HUD;
	UCharacterMenuWidget* CharacterMenu;
	URogueHUD* RogueHUD;

	//UPROPERTY()
	//TArray<UUpgradeEntryData*> UpgradeItems;

	ResourceUpgrade resourceUpgrades[UPGRADE_COUNT];

	// ROGUE //
	bool inRun = false;
	RoguePlayerData rogueData;

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
	void ApplyResourceUpgrade(UpgradeType _upgrade, int _level);
	FString CreateCostText(UpgradeType _upgrade, const TArray<ResourceData>& _cost);
	void RebuildCostCache(UpgradeType _upgrade);
	int GetMaxUpgradeMult(UpgradeType _upgrade);

	bool resourcesDirty = false;

	void MoveForward();

	UFUNCTION()
	void FocusIdle();

	UFUNCTION()
	void FocusRogue();

	UFUNCTION()
	void FocusMenu();
	
	UFUNCTION()
	void StartRun();

	UFUNCTION()
	void EndRun();

	UFUNCTION()
	void ShowCharacterMenu();

	UFUNCTION()
	void HideCharacterMenu();

	UFUNCTION()
	void UpgradeStrongArms();
	UFUNCTION()
	void UpgradeDrill();
	UFUNCTION()
	void UpgradeBoom();

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

	ADwarfPawn* currentPawn;
	ADwarfPawn* idlePawn;
	Cave* currentCave = nullptr;
	Cave idleCave;

	// Dwarf Stats
	int Level;
	int Experience;
	int RequiredExperience;
	void IncreaseExp(int _value);
	void LevelUp();
	void IncreaseRogueExp(int _value);
	int TalentPoints;

	float GlobalYieldMultiplier = 1.0f;
	float resourceYieldMultiplier[RESOURCE_COUNT];
	float blockYieldMultiplier[BLOCK_COUNT];

	FString GetUpgradeDamageText(UpgradeType _upgrade);

	DamageSource ClickSource;
	int MinDamage = 10;
	int MaxDamage = 15;
	int GetClickDamage();

	AutomaticDamager Drill;
	AutomaticDamager Boom;

	void UpdateDamager(AutomaticDamager& _damager, float _dt);

	void CreateDamageText(int _damage, DamageTextType _type);
	void Hit();
	void Damage(int _damage, DamageSource _source, Cave* _cave);
	void DamageIdleCave(int _damage, DamageSource _source);

	void BlockRewardIdle(BlockData _data);
	void BlockRewardRogue(BlockData _data);


	// Stored stats
	SavedStats savedStats;
	
	void IncreaseBlocks();
	void IncreaseWalk();

	int resources[RESOURCE_COUNT];
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
