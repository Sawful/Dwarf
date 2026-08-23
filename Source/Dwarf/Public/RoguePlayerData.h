// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigNumber.h"
#include "Cave.h"
#include "RogueItem.h"
#include "UpgradeEntryWidget.h"
#include "DamageSource.h"
#include "RogueItem.h"
#include "RoguePlayerData.generated.h"

class ADwarfPlayerState;
class URogueHUD;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageCalc, BigNumber&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPressureCalc, BigNumber&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPressureRegenCalc, BigNumber&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnExpCalc, BigNumber&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCooldownCalc, float&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTick, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FRogueOnHit, URoguePlayerData*);

UCLASS(BlueprintType)
class DWARF_API URoguePlayerData: public UObject
{
	GENERATED_BODY()
public:
	void Pause() { timePaused = true; }
	void Resume() { timePaused = false; }

	void Update(float _dt);
	BigNumber GetClickDamage();
	void RegeneratePressure(BigNumber _baseValue);

	void BlockReward(BlockData _data);
	BigNumber GetExp(BigNumber _exp);
	void IncreaseExp(BigNumber _exp);
	void RogueCardsPopup();

	UFUNCTION(BlueprintCallable)
	void SelectItem(int _index);

	void AddItem(URogueItem* _item);
	void AddItemFromPool(unsigned int _index);

	URogueHUD* RogueHUD;

	Cave* currentCave;

	int level = 1;
	BigNumber experience = 0;
	BigNumber experienceRequired = 8;
	TArray<URogueItem*> items;
	TArray<UUpgradeEntryWidget*> itemWidgets;

	TArray<URogueItem*> currentItemPool;
	int itemsSelection[3];

	float clickCooldown = 1.0f;
	float clickTimer = 1.0f;

	DamageSource ClickSource;
	BigNumber MinDamage = 10;
	BigNumber MaxDamage = 15;
	BigNumber MaxPressure = 100;
	BigNumber Pressure = 100;
	float PressureResistance = 0; // 0 <=> full damage, 1 <=> invulnerable

	FOnDamageCalc OnDamageCalc;
	FOnPressureCalc OnPressureCalc;
	FOnPressureRegenCalc OnPressureRegenCalc;
	FOnExpCalc OnExpCalc;
	FOnCooldownCalc OnCooldownCalc;
	FOnTick OnTick;

	void LevelUpRogue();
	float GetHitCooldown();

private:
	bool timePaused = false;
};
