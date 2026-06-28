// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/TextRenderActor.h"
#include "DwarfUserWidget.h"
#include "Block.h"
#include "DwarfPlayerState.generated.h"

class Cave;
class USaveGame;
class ADwarfPawn;

enum UpgradeType
{
	STRONG_ARMS = 0,
	DRILL,

	UPGRADE_COUNT
};

struct ResourceUpgrades
{
	UPROPERTY()
	FString upgradeFunctionNames[UPGRADE_COUNT];

	UPROPERTY()
	FString displayNames[UPGRADE_COUNT];

	UPROPERTY()
	int upgradeLevels[UPGRADE_COUNT];
	TArray<ResourceData> GetCost(UpgradeType _upgrade);
};

struct SavedStats
{
	int damageDone = 0;
	int blocksBroken = 0;
	int metersWalked = 0;
};

UCLASS()
class DWARF_API ADwarfPlayerState : public APlayerState
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere)
	TSubclassOf <ATextRenderActor> DamageTextClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDwarfUserWidget> HUDClass;

	UDwarfUserWidget* HUD;

	ResourceUpgrades resourceUpgrades;

public:
	void OnSaveFinished(const FString& _name, const int32 _userIndex, bool _success);
	void OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	UFUNCTION()
	void SaveCurrentState();

	bool CheckCost(const TArray<ResourceData>& _cost);
	bool PayCost(const TArray<ResourceData>& _cost);
	void BuyResourceUpgrade(UpgradeType _upgrade);
	void IncreaseResourceUpgrade(UpgradeType _upgrade);

	void MoveForward();

	UFUNCTION()
	void UpgradeStrongArms();
	UFUNCTION()
	void UpgradeDrill();
	
	ADwarfPawn* pawn;
	Cave* cave;

	float movementSpeed = 3;

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

	void CreateDamageText(int _damage);
	void Hit();
	void Damage(int _damage);

	// Stored stats
	SavedStats savedStats;
	
	void IncreaseBlocks();
	void IncreaseWalk();

	int resources[RESOURCE_COUNT];
};
