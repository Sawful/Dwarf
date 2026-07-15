// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/TextRenderActor.h"

#include "DwarfCameraActor.h"

#include "MainMenuWidget.h"
#include "DwarfUserWidget.h"
#include "CharacterMenuWidget.h"
#include "RogueHUD.h"

#include "Block.h"
#include "Cave.h"
#include "DwarfPawn.h"
#include "DwarfPlayerState.generated.h"

class USaveGame;
class UUpgradeEntryData;

enum UpgradeType
{
	STRONG_ARMS = 0,
	DRILL,
	BOOM,

	UPGRADE_COUNT
};

DECLARE_DELEGATE_RetVal_OneParam(TArray<ResourceData>, FOnGetCost, int)
struct ResourceUpgrade
{
	UPROPERTY()
	FString upgradeFunctionName;

	UPROPERTY()
	FString displayName;

	UPROPERTY()
	int upgradeLevel;

	UPROPERTY()
	TArray<ResourceData> costCached;

	FOnGetCost costDelegate;

	TArray<ResourceData> GetCost();

};

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
	UExpBarWidget* progressBar;
	bool IsHitting();
	float GetDPS() { return (float)Damage / Downtime; };
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
	bool inRun = false;

	// Main camera (follows the dwarf)
	UPROPERTY(EditAnywhere)
	ADwarfCameraActor* CameraActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf <ATextRenderActor> DamageTextClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuWidget> MenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDwarfUserWidget> HUDClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterMenuWidget> CharacterMenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URogueHUD> RogueHUDClass;

	UMainMenuWidget* MainMenu; 
	UDwarfUserWidget* HUD;
	UCharacterMenuWidget* CharacterMenu;
	URogueHUD* RogueHUD;

	UPROPERTY()
	TArray<UUpgradeEntryData*> UpgradeItems;

	ResourceUpgrade resourceUpgrades[UPGRADE_COUNT];

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

	void MoveForward();

	UFUNCTION()
	void ZoomIdle();

	UFUNCTION()
	void ZoomRogue();

	UFUNCTION()
	void ZoomMenu();
	
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
