// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfPlayerState.h"
#include "DwarfPawn.h"
#include "Cave.h"
#include "DwarfUserWidget.h"
#include "Components/TextRenderComponent.h"
#include "UpgradeEntryData.h"
#include "DwarfSaveGame.h"

ADwarfPlayerState::ADwarfPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADwarfPlayerState::SetupResourceUpgrades()
{
	resourceUpgrades[0].upgradeFunctionName = "StrongArms";
	resourceUpgrades[0].displayName = "Strong Arms";
	SetupResourceUpgradeDelegate(resourceUpgrades[0], &CostStrongArms);

	resourceUpgrades[1].upgradeFunctionName = "Drill";
	resourceUpgrades[1].displayName = "Drill";
	SetupResourceUpgradeDelegate(resourceUpgrades[1], &CostDrill);
}

void ADwarfPlayerState::SetupResourceUpgradeDelegate(ResourceUpgrade& upgrade, TArray<ResourceData> (*InFunc)(int))
{
	FOnGetCost del;
	FString costDelName = "Cost";
	costDelName += upgrade.upgradeFunctionName;
	del.BindStatic(InFunc);
	upgrade.costDelegate = del;
}

void ADwarfPlayerState::SetupMilestones()
{
	savedStats.blocksBroken.maximumTier = 4;
	savedStats.blocksBroken.tiers.Push(15);
	savedStats.blocksBroken.tiers.Push(50);
	savedStats.blocksBroken.tiers.Push(250);
	savedStats.blocksBroken.tiers.Push(1500);
	savedStats.blocksBroken.tierUpDelegate.BindUObject(this, &ADwarfPlayerState::BlockMilestone);
}

void ADwarfPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ADwarfPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SaveCurrentState(); // Doesn't work obv
}

void ADwarfPlayerState::Tick(float _dt)
{
	Super::Tick(_dt);
	if (gameLoaded == false)
	{
		return;
	}

	DrillClock -= _dt;

	if (DrillClock <= 0)
	{
		DrillClock = DrillDowntime;

		CreateDamageText(DrillDamage, AUTO);
		Damage(DrillDamage);
	}
}

void ADwarfPlayerState::StartGame()
{
	gameLoaded = true;

	SetupMilestones();
	SetupResourceUpgrades();

	if (MenuClass)
	{
		MainMenu = CreateWidget<UMainMenuWidget>(GetPlayerController(), MenuClass);

		FScriptDelegate idleClickDelegate;
		idleClickDelegate.BindUFunction(this, "ZoomIdle");
		MainMenu->IdleButton->OnClicked.Add(idleClickDelegate);
		MainMenu->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create Main Menu")));
	}

	if (HUDClass)
	{
		HUD = CreateWidget<UDwarfUserWidget>(GetPlayerController(), HUDClass);

		// Setup Upgrade delegates and buttons
		for (int i = 0; i < UPGRADE_COUNT; i++)
		{
			FScriptDelegate delegate;
			FName delegateName;
			delegateName = FName(resourceUpgrades[i].upgradeFunctionName);
			FWideString prefix = "Upgrade";
			delegateName.AppendString(prefix);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, prefix);
			delegate.BindUFunction(this, FName(prefix));

			UUpgradeEntryData* data = NewObject<UUpgradeEntryData>(this);
			data->upgradeName = resourceUpgrades[i].displayName;
			data->upgradeDelegate = delegate;

			HUD->UpgradeBox->AddItem(data);
		}

		HUD->SaveButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::SaveCurrentState);
		HUD->RebirthButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::Rebirth);
		HUD->MenuButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::ZoomMenu);
		HUD->CharacterMenuButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::ShowCharacterMenu);
		HUD->Populate();
		HUD->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create HUD")));
	}

	if (CharacterMenuClass)
	{
		CharacterMenu = CreateWidget<UCharacterMenuWidget>(GetPlayerController(), CharacterMenuClass);
		CharacterMenu->ExitButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::HideCharacterMenu);
		CharacterMenu->SetVisibility(ESlateVisibility::Hidden);
		CharacterMenu->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create CharacterMenuClass")));
	}
	CameraActor = GetWorld()->SpawnActor<ADwarfCameraActor>(ADwarfCameraActor::StaticClass(), FVector(), FRotator(), FActorSpawnParameters());
	CameraActor->pawn = pawn;
	ZoomMenu();
	CameraActor->ForcePos();

	currentCave = new Cave();
	currentCave->player = this;
	currentCave->GenerateStart();

	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		resourceYieldMultiplier[i] = 1.0f;
	}
	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		blockYieldMultiplier[i] = 1.0f;
	}

	// Load
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &ADwarfPlayerState::OnLoadFinished);
	UGameplayStatics::AsyncLoadGameFromSlot("SaveSlot", 0, LoadedDelegate);
}

void ADwarfPlayerState::OnSaveFinished(const FString& _name, const int32 _userIndex, bool _success)
{
	if (_success)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString(TEXT("Finished saving!")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("Save failed...")));
	}

}

void ADwarfPlayerState::OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UDwarfSaveGame* save = Cast<UDwarfSaveGame>(LoadedGameData);
	if (save == nullptr)
	{
		// Default first time init
		ResetDwarfStats();
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString(TEXT("First time launching the game.")));
		return;
	}

	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT;)
	{
		resourceUpgrades[upgrade].upgradeLevel = save->upgradeLevels[upgrade];	// Get level
		for (int i = 0; i < resourceUpgrades[upgrade].upgradeLevel; i++)		// Re-apply upgrade
		{
			IncreaseResourceUpgrade(upgrade);
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(resourceUpgrades[upgrade].upgradeLevel));
		upgrade = (UpgradeType)(1 + upgrade);
	}

	DrillClock = DrillDowntime;

	// Resources
	memcpy(resources, save->resources, sizeof(int) * RESOURCE_COUNT);
	HUD->UpdateResources(resources);

	// Saved Stats (TODO: find a way to put these into a list)
	savedStats.blocksBroken.value = save->savedStats[0];
	savedStats.blocksBroken.CheckTier();
	savedStats.damageDone.value = save->savedStats[1];
	savedStats.damageDone.CheckTier();
	savedStats.metersWalked.value = save->savedStats[2];
	savedStats.metersWalked.CheckTier();
	savedStats.rebirthCount.value = save->savedStats[3];
	savedStats.rebirthCount.CheckTier();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString(TEXT("Finished loading!")));
}

void ADwarfPlayerState::ResetDwarfStats()
{
	MinDamage = 10;
	MaxDamage = 15;
	DrillDamage = 0;
	DrillDowntime = 2;

	// Re-apply milestone buffs
	savedStats.blocksBroken.currentTier = 0;
	savedStats.blocksBroken.CheckTier();
	savedStats.damageDone.currentTier = 0;
	savedStats.damageDone.CheckTier();
	savedStats.metersWalked.currentTier = 0;
	savedStats.metersWalked.CheckTier();
	savedStats.rebirthCount.currentTier = 0;
	savedStats.rebirthCount.CheckTier();
}

void ADwarfPlayerState::SaveCurrentState()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString(TEXT("Saving...")));

	FAsyncSaveGameToSlotDelegate SavedDelegate;
	SavedDelegate.BindUObject(this, &ADwarfPlayerState::OnSaveFinished);

	UDwarfSaveGame* save = (UDwarfSaveGame*)UGameplayStatics::CreateSaveGameObject(UDwarfSaveGame::StaticClass());
	// Set all values
	
	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT; upgrade = (UpgradeType)(upgrade + 1))
	{
		save->upgradeLevels[upgrade] = resourceUpgrades[upgrade].upgradeLevel;	// Set level
	}
	memcpy(save->resources, resources, sizeof(int) * RESOURCE_COUNT);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(save->upgradeLevels[0]));
	save->savedStats[0] = savedStats.blocksBroken.value;
	save->savedStats[1] = savedStats.damageDone.value;
	save->savedStats[2] = savedStats.metersWalked.value;
	save->savedStats[3] = savedStats.rebirthCount.value;

	UGameplayStatics::AsyncSaveGameToSlot(save, "SaveSlot", 0, SavedDelegate);
}

void ADwarfPlayerState::Rebirth()
{
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i].upgradeLevel = 0;
	}
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		resources[i] = 0;
	}

	ResetDwarfStats();
	currentCave->ResetCave();
	pawn->ResetDwarfPawn();
	HUD->UpdateResources(resources);

	savedStats.rebirthCount.value++;
	savedStats.rebirthCount.CheckTier();
}

bool ADwarfPlayerState::CheckCost(const TArray<ResourceData>& _cost)
{
	for (auto res : _cost)
	{
		if (resources[res.Type] < res.Amount) return false;
	}
	return true;
}

bool ADwarfPlayerState::PayCost(const TArray<ResourceData>& _cost)
{
	if (CheckCost(_cost) == false) return false;
	for (auto res : _cost)
	{
		resources[res.Type] -= res.Amount;
	}

	HUD->UpdateResources(resources);

	return true;
}

void ADwarfPlayerState::BuyResourceUpgrade(UpgradeType _upgrade)
{
	TArray<ResourceData> cost = resourceUpgrades[_upgrade].GetCost();
	if (PayCost(cost) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Not enough resources"); 
		return;
	}

	// Give upgrade reward based on type
	resourceUpgrades[_upgrade].upgradeLevel++;
	IncreaseResourceUpgrade(_upgrade);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Bought upgrade " + FString::FromInt(_upgrade));
}

void ADwarfPlayerState::IncreaseResourceUpgrade(UpgradeType _upgrade)
{
	switch (_upgrade)
	{
	case STRONG_ARMS:
	{
		MinDamage += 1;
		MaxDamage += 2;
		break;
	}
	case DRILL:
	{
		DrillDamage += 1;
		DrillDowntime *= 0.9;
		break;
	}
	}
}

void ADwarfPlayerState::UpgradeStrongArms()
{
	BuyResourceUpgrade(STRONG_ARMS);
}

TArray<ResourceData> CostStrongArms(int _level)
{
	TArray<ResourceData> cost;
	cost.Add({ DIRT, 1 + _level });
	cost.Add({ STONE, 1 + 2 * _level });
	return cost;
}

TArray<ResourceData> CostDrill(int _level)
{
	TArray<ResourceData> cost;
	cost.Add({ ORE, 5 + 2 * _level });
	return cost;
}

void ADwarfPlayerState::UpgradeDrill()
{
	BuyResourceUpgrade(DRILL);
}

void ADwarfPlayerState::BlockMilestone(int _tier)
{
	// Check milestones:
	switch (_tier)
	{
	case 0:
	{
		MinDamage += 5;
		MaxDamage += 5;
		break;
	}
	case 1:
	{
		MinDamage += 10;
		MaxDamage += 10;
		break;
	}
	case 2:
	{
		MinDamage += 15;
		MaxDamage += 15;
		break;
	}
	case 3:
	{
		MinDamage += 20;
		MaxDamage += 20;
		break;
	}
	}
}

void ADwarfPlayerState::IncreaseExp(int _value)
{
	Experience += _value;
	while (Experience >= RequiredExperience)
	{
		Experience -= RequiredExperience;

		// Increase level
		LevelUp();
	}

	HUD->ExpBar->SetCompletion((float)Experience / (float)RequiredExperience);
}

void ADwarfPlayerState::LevelUp()
{
	Level++;
	// Do stuff per level (ex: Give tech point, check for level milestone, etc.)
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, "Level up");
	TalentPoints++;
	// Show the tech point
	HUD->TechPointIndicator->SetVisibility(ESlateVisibility::Visible);

	// Compute new Req exp
	RequiredExperience = (Level + 1) * (Level + 1);
}

int ADwarfPlayerState::GetClickDamage()
{
	int DamageDelta = MaxDamage - MinDamage;
	int DamageBonus = rand() % (DamageDelta + 1);
	return MinDamage + DamageBonus;
}

void ADwarfPlayerState::Hit()
{
	int damage = GetClickDamage();
	if (damage == MaxDamage)
	{
		CreateDamageText(damage, CRITICAL);
	}
	else
	{
		CreateDamageText(damage, NORMAL);
	}
	Damage(damage);
}

void ADwarfPlayerState::Damage(int _damage)
{
	if (!currentCave) return;
	if (!currentCave->first) return;

	BlockData currentBlockData = currentCave->first->Data;
	if (currentCave->DamageFirst(_damage))
	{
		IncreaseBlocks();

		//DEBUG PURPOSES ONLY
		IncreaseExp(1);

		for (auto currentYield : currentBlockData.yield)
		{
			resources[currentYield.Type] += currentYield.Amount * GlobalYieldMultiplier * resourceYieldMultiplier[currentYield.Type];
		}

		HUD->UpdateResources(resources);
	}
}

void ADwarfPlayerState::MoveForward()
{
	IncreaseWalk();
	pawn->MoveForward();
}

void ADwarfPlayerState::SetRogue()
{
	currentCave->SetCaveVisible(false);
	pawn->SetActorHiddenInGame(true);
}

void ADwarfPlayerState::ZoomIdle()
{
	if (CameraActor) CameraActor->SetState(ADwarfCameraActor::IDLE);
	MainMenu->SetVisibility(ESlateVisibility::Hidden);
	HUD->SetVisibility(ESlateVisibility::Visible);
}

void ADwarfPlayerState::ZoomMenu()
{
	if(CameraActor) CameraActor->SetState(ADwarfCameraActor::MENU);
	HUD->SetVisibility(ESlateVisibility::Hidden);
	MainMenu->SetVisibility(ESlateVisibility::Visible);
}

void ADwarfPlayerState::ShowCharacterMenu()
{
	CharacterMenu->SetVisibility(ESlateVisibility::Visible);
}

void ADwarfPlayerState::HideCharacterMenu()
{
	CharacterMenu->SetVisibility(ESlateVisibility::Hidden);
}

void ADwarfPlayerState::IncreaseBlocks()
{
	savedStats.blocksBroken.value++;
	savedStats.blocksBroken.CheckTier();
}

void ADwarfPlayerState::IncreaseWalk()
{
	savedStats.metersWalked.value++;
	savedStats.metersWalked.CheckTier();
}

void ADwarfPlayerState::CreateDamageText(int _damage, DamageTextType _type)
{
	if (!DamageTextClass) return;
	if (!currentCave) return;
	if (!currentCave->first) return;

	FVector position = currentCave->first->GetActorLocation();
	position.X += -55;
	position.Y += ((float)((rand() % 100) - 50));
	position.Z += ((float)((rand() % 100) - 50));

	FRotator rotator;
	rotator.Yaw = 180;
	ATextRenderActor* damageText = GetWorld()->SpawnActor<ATextRenderActor>(DamageTextClass, position, rotator, FActorSpawnParameters());
	UTextRenderComponent* textRender = damageText->GetTextRender();
	switch (_type)
	{
	case NORMAL:
	{
		textRender->SetTextRenderColor(FColor(0xFFFF0000));
		textRender->WorldSize = 40;
		break;
	}
	case AUTO:
	{
		textRender->SetTextRenderColor(FColor(0xFF0F0F0F));
		textRender->WorldSize = 30;
		break;
	}
	case CRITICAL:
	{
		textRender->SetTextRenderColor(FColor(0xFFF0F000));
		textRender->WorldSize = 60;
		break;
	}

	}
		
	textRender->SetText(FText::FromString(FString::FromInt(_damage)));
}

TArray<ResourceData> ResourceUpgrade::GetCost()
{
	costCached = costDelegate.Execute(upgradeLevel);
	return costCached;
}
