// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/TextRenderActor.h"

#include "DwarfCameraActor.h"
#include "DwarfUserWidget.h"
#include "MainMenuWidget.h"
#include "Block.h"
#include "Cave.h"
#include "DwarfPlayerState.generated.h"

class USaveGame;
class ADwarfPawn;

enum UpgradeType
{
	STRONG_ARMS = 0,
	DRILL,

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

	// Main camera (follows the dwarf)
	UPROPERTY(EditAnywhere)
	ADwarfCameraActor* CameraActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf <ATextRenderActor> DamageTextClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDwarfUserWidget> HUDClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuWidget> MenuClass;

	UDwarfUserWidget* HUD;
	UMainMenuWidget* MainMenu;

	ResourceUpgrade resourceUpgrades[UPGRADE_COUNT];

public:
	void OnSaveFinished(const FString& _name, const int32 _userIndex, bool _success);
	void OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	
	void ResetDwarfStats();

	UFUNCTION()
	void SaveCurrentState();

	UFUNCTION()
	void Rebirth();

	bool CheckCost(const TArray<ResourceData>& _cost);
	bool PayCost(const TArray<ResourceData>& _cost);
	void BuyResourceUpgrade(UpgradeType _upgrade);
	void IncreaseResourceUpgrade(UpgradeType _upgrade);

	void MoveForward();

	UFUNCTION()
	void SetRogue();

	UFUNCTION()
	void ZoomIdle();
	UFUNCTION()
	void ZoomMenu();

	UFUNCTION()
	void UpgradeStrongArms();

	UFUNCTION()
	void UpgradeDrill();

	UFUNCTION()
	void BlockMilestone(int _tier);
	
	ADwarfPawn* pawn;
	Cave* currentCave;
	Cave caves[2];

	float GlobalYieldMultiplier = 1.0f;
	float resourceYieldMultiplier[RESOURCE_COUNT];
	float blockYieldMultiplier[BLOCK_COUNT];

	int MinDamage = 10;
	int MaxDamage = 15;
	int GetClickDamage();

	// Drill data
	int DrillDamage = 0;
	float DrillDowntime = 2;
	float DrillClock;

	enum DamageTextType
	{
		NORMAL,
		AUTO,
		CRITICAL
	};
	void CreateDamageText(int _damage, DamageTextType _type);
	void Hit();
	void Damage(int _damage);

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
