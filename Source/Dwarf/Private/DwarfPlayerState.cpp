// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfPlayerState.h"
#include "RogueCave.h"
#include "IdleHUD.h"
#include "Components/TextRenderComponent.h"
#include "Components/TextBlock.h"
#include "UpgradeEntryData.h"
#include "DwarfSaveGame.h"

#define MAX_MULTIPLIER_VALUE 10000

ADwarfPlayerState::ADwarfPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		resourceYieldMultiplier[i] = 1.0f;
	}
	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		blockYieldMultiplier[i] = 1.0f;
	}
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		upgradeMultiplier[i] = 1;
	}
}

void ADwarfPlayerState::SetupResourceUpgrades()
{
	resourceUpgrades[0].upgradeFunctionName = "StrongArms";
	resourceUpgrades[0].displayName = "Strong Arms";
	resourceUpgrades[0].widget = HUD->ClickUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[0], &CostStrongArms);

	resourceUpgrades[1].upgradeFunctionName = "Drill";
	resourceUpgrades[1].displayName = "Drill";
	resourceUpgrades[1].widget = HUD->DrillUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[1], &CostDrill);

	resourceUpgrades[2].upgradeFunctionName = "Boom";
	resourceUpgrades[2].displayName = "Boom";
	resourceUpgrades[2].widget = HUD->BoomUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[2], &CostBoom);

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
		resourceUpgrades[i].widget->Button->OnClicked.Add(delegate);
		resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
		resourceUpgrades[i].UpdateTooltipText();
		/*UUpgradeEntryData* data = NewObject<UUpgradeEntryData>(this);
		data->upgradeName = resourceUpgrades[i].displayName;
		data->upgradeDelegate = delegate;
		data->SetLevel(resourceUpgrades[i].upgradeLevel);
		data->SetCostText(CreateCostText((UpgradeType)i, resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i])));
		data->SetDamageText(GetUpgradeDamageText((UpgradeType)i));
		UpgradeItems.Add(data);
		HUD->UpgradeBox->AddItem(data);*/
	}
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

void ADwarfPlayerState::InitializeAutomaticDamagers()
{
	Drill.Damage = 0;
	Drill.Downtime = 2.0f;
	Drill.Clock = Drill.Downtime;

	Drill.Display = HUD->DrillDisplay;
	Drill.Display->ProgressBar->SetCompletion((Drill.Downtime - Drill.Clock) / Drill.Downtime);

	Drill.SetDamagerActive(false);
	Drill.source.TextType = AUTO;

	Drill.source.BlockDamageMultiplier[ORE_BLOCK] = 5; // TESTING

	Boom.Damage = 0;
	Boom.Downtime = 10.0f;
	Boom.Clock = Boom.Downtime;

	Boom.Display = HUD->BoomDisplay;
	Boom.Display->ProgressBar->SetCompletion((Boom.Downtime - Boom.Clock) / Boom.Downtime);

	Boom.SetDamagerActive(false);
	Boom.source.TextType = AUTO;
}

void ADwarfPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ADwarfPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SaveCurrentState(); // Doesn't work obv
}

void ADwarfPlayerState::Tick(float _dt)
{
	Super::Tick(_dt);
	if (gameLoaded == false)
	{
		return;
	}

	// Check upgrade costs
	// If mult is Max, find "max"
	if (maxMultiplier)
	{
		for (int i = 0; i < UPGRADE_COUNT; i++)
		{
			if (resourcesDirty)
			{
				upgradeMultiplier[i] = GetMaxUpgradeMult((UpgradeType)i);
			}

			resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
		}
	}

	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		bool upgradeAvailable = true;
		FString string = "Cost (x" + FString::FromInt(upgradeMultiplier[i]) + "): \n";
		for (auto resource : resourceUpgrades[i].costCached)
		{
			bool resourceAvailable = true;

			string += GetResourceIcon(resource.Type);
			if (resources[resource.Type] < resource.Amount)
			{
				resourceAvailable = false;
				upgradeAvailable = false;
				string += "<UnavailableEmphasis>";
			}
			string += ": ";
			string += FString::FromInt(resource.Amount);
			if (resourceAvailable == false) string += "</>";
			string += "\n";

		}

		if (upgradeAvailable)
		{
			resourceUpgrades[i].widget->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		}
		else
		{
			resourceUpgrades[i].widget->SetColorAndOpacity(FLinearColor(0.1, 0.1, 0.1, 1));
		}
		resourceUpgrades[i].widget->costText = string;
	}

	// Idle auto damagers
	UpdateDamager(Drill, _dt);
	UpdateDamager(Boom, _dt);

	if (inRun == false) return;
	// Run auto damagers

}

void ADwarfPlayerState::StartGame()
{
	GlobalYieldMultiplier = 100000;

	if (RogueHUDClass)
	{
		RogueHUD = CreateWidget<URogueHUD>(GetPlayerController(), RogueHUDClass);

		RogueHUD->MenuButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::EndRun);

		RogueHUD->SetVisibility(ESlateVisibility::Hidden);
		RogueHUD->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create RogueHUD")));
		return;
	}

	if (MenuClass)
	{
		MainMenu = CreateWidget<UMainMenuWidget>(GetPlayerController(), MenuClass);

		MainMenu->IdleButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::FocusIdle);
		MainMenu->RunButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::StartRun);

		MainMenu->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create Main Menu")));
		return;
	}

	if (HUDClass)
	{
		HUD = CreateWidget<UIdleHUD>(GetPlayerController(), HUDClass);
		if(IsValid(HUD))
		{
			HUD->SaveButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::SaveCurrentStateAsync);
			HUD->RebirthButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::Rebirth);
			HUD->MenuButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::FocusMenu);
			HUD->CharacterMenuButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::ShowCharacterMenu);

			HUD->ButtonMult1->OnClicked.AddDynamic(this, &ADwarfPlayerState::SetUpgradeMult1);
			HUD->ButtonMult5->OnClicked.AddDynamic(this, &ADwarfPlayerState::SetUpgradeMult5);
			HUD->ButtonMult10->OnClicked.AddDynamic(this, &ADwarfPlayerState::SetUpgradeMult10);
			HUD->ButtonMult25->OnClicked.AddDynamic(this, &ADwarfPlayerState::SetUpgradeMult25);
			HUD->ButtonMult100->OnClicked.AddDynamic(this, &ADwarfPlayerState::SetUpgradeMult100);
			HUD->ButtonMultMax->OnClicked.AddDynamic(this, &ADwarfPlayerState::SetUpgradeMultMax);

			HUD->Populate();
			HUD->AddToViewport();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create HUD")));
		return;
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
		return;
	}

	InitializeAutomaticDamagers();
	SetupMilestones();

	ClickSource.TextType = NORMAL;

	currentPawn = idlePawn;

	CameraActor = GetWorld()->SpawnActor<ADwarfCameraActor>(ADwarfCameraActor::StaticClass(), FVector(), FRotator(), FActorSpawnParameters());
	CameraActor->pawn = currentPawn;
	CameraActor->ForcePos();

	idleCave.dwarfPawn = idlePawn;
	idleCave.BlockBreakDelegate.BindUObject(this, &ADwarfPlayerState::BlockRewardIdle);
	idleCave.GenerateStart();
	currentCave = &idleCave;

	FocusMenu();

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
	gameLoaded = true;
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
		for (int i = 1; i <= resourceUpgrades[upgrade].upgradeLevel; i++)		// Re-apply upgrade
		{
			ApplyResourceUpgrade(upgrade, i);
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(resourceUpgrades[upgrade].upgradeLevel));
		upgrade = (UpgradeType)(1 + upgrade);
	}

	SetupResourceUpgrades();

	Drill.Clock = Drill.Downtime;

	// Resources
	memcpy(resources, save->resources, sizeof(int) * RESOURCE_COUNT);
	HUD->UpdateResources(resources);
	resourcesDirty = true;

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
	InitializeAutomaticDamagers();

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

void ADwarfPlayerState::SaveCurrentStateAsync()
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

void ADwarfPlayerState::SaveCurrentState()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString(TEXT("Saving...")));

	UDwarfSaveGame* save = (UDwarfSaveGame*)UGameplayStatics::CreateSaveGameObject(UDwarfSaveGame::StaticClass());
	
	//// Set all values ////

	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT; upgrade = (UpgradeType)(upgrade + 1))
	{
		save->upgradeLevels[upgrade] = resourceUpgrades[upgrade].upgradeLevel;	// Set level
	}

	memcpy(save->resources, resources, sizeof(int) * RESOURCE_COUNT);
	save->savedStats[0] = savedStats.blocksBroken.value;
	save->savedStats[1] = savedStats.damageDone.value;
	save->savedStats[2] = savedStats.metersWalked.value;
	save->savedStats[3] = savedStats.rebirthCount.value;

	UGameplayStatics::SaveGameToSlot(save, "SaveSlot", 0);
}

void ADwarfPlayerState::Rebirth()
{
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i].upgradeLevel = 0;
		resourceUpgrades[i].UpdateTooltipText();
		//UpgradeItems[i]->SetCostText(CreateCostText((UpgradeType)i, resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i])));
		//UpgradeItems[i]->SetDamageText(GetUpgradeDamageText((UpgradeType)i));
	}
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		resources[i] = 0;
	}

	ResetDwarfStats();
	currentCave->ResetCave();
	idlePawn->ResetDwarfPawn();
	HUD->UpdateResources(resources);
	resourcesDirty = true;

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
	resourcesDirty = true;

	return true;
}

void ADwarfPlayerState::BuyResourceUpgrade(UpgradeType _upgrade)
{
	TArray<ResourceData> cost = resourceUpgrades[_upgrade].GetCostAndCache(upgradeMultiplier[_upgrade]);
	if (PayCost(cost) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Not enough resources"); 
		return;
	}

	// Give upgrade reward based on type
	for (int i = 0; i < upgradeMultiplier[_upgrade]; i++)
	{
		resourceUpgrades[_upgrade].upgradeLevel++;
		ApplyResourceUpgrade(_upgrade, resourceUpgrades[_upgrade].upgradeLevel);
	}
	
	resourceUpgrades[_upgrade].UpdateTooltipText();
	resourceUpgrades[_upgrade].GetCostAndCache(upgradeMultiplier[_upgrade]);
	//UpgradeItems[_upgrade]->SetDamageText(GetUpgradeDamageText(_upgrade));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Bought upgrade " + FString::FromInt(_upgrade));
}

void ADwarfPlayerState::ApplyResourceUpgrade(UpgradeType _upgrade, int _level)
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
		Drill.Damage += 1;
		switch (_level)
		{
		case 1:
			Drill.SetDamagerActive(true);
			break;
		case 10:
			Drill.Damage *= 2;
			Drill.Downtime *= 0.8;
			break;
		case 25:
			Drill.Damage *= 3;
			Drill.Downtime *= 0.8;
			break;
		case 50:
			Drill.Damage *= 5;
			Drill.Downtime *= 0.8;
			break;
		}
		break;
	}
	case BOOM:
	{
		Boom.Damage += 10;
		switch (_level)
		{
		case 1:
			Boom.SetDamagerActive(true);
			break;
		case 10:
			Boom.Damage *= 2;
			Boom.Downtime *= 0.8;
			break;
		case 25:
			Boom.Damage *= 3;
			Boom.Downtime *= 0.8;
			break;
		case 50:
			Boom.Damage *= 5;
			Boom.Downtime *= 0.8;
			break;
		}

		break;
	}
	}
}

FString ADwarfPlayerState::CreateCostText(UpgradeType _upgrade, const TArray<ResourceData>& _cost)
{
	FString string = "Cost (x" + FString::FromInt(upgradeMultiplier[_upgrade]) + "): \n";
	for (auto resource : _cost)
	{
		bool resourceAvailable = true;

		string += GetResourceIcon(resource.Type);
		if (resources[resource.Type] < resource.Amount)
		{
			resourceAvailable = false;
			string += "<UnavailableEmphasis>";
		}
		string += ": ";
		string += FString::FromInt(resource.Amount);
		if (resourceAvailable == false) string += "</>";
		string += "\n";

	}

	return string;
}

int ADwarfPlayerState::GetMaxUpgradeMult(UpgradeType _upgrade)
{
	TArray<ResourceData> cost;
	cost = resourceUpgrades[_upgrade].costDelegate.Execute(resourceUpgrades[_upgrade].upgradeLevel);
	int maxMult = 1;

	int typeCount = cost.Num();
	for (int i = 1; i < MAX_MULTIPLIER_VALUE; i++)
	{
		TArray<ResourceData> currentCost = resourceUpgrades[_upgrade].costDelegate.Execute(resourceUpgrades[_upgrade].upgradeLevel + i);
		for (int j = 0; j < typeCount; j++)
		{
			cost[j].Amount += currentCost[j].Amount;
			if (resources[cost[j].Type] < cost[j].Amount)
			{
				return maxMult;
			}
		}
		maxMult++;
	}

	return maxMult;
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
	cost.Add({ ORE, 1 + 2 * _level });
	cost.Add({ ORE2, 1 + 2 * _level });
	cost.Add({ ORE3, 1 + 2 * _level });
	cost.Add({ ORE4, 1 + 2 * _level });
	cost.Add({ ORE5, 1 + 2 * _level });
	return cost;
}

TArray<ResourceData> CostDrill(int _level)
{
	TArray<ResourceData> cost;
	cost.Add({ ORE, 5 + 2 * _level });
	return cost;
}

TArray<ResourceData> CostBoom(int _level)
{
	TArray<ResourceData> cost;
	cost.Add({ ORE, 5 + 2 * _level });
	return cost;
}

void ADwarfPlayerState::UpgradeDrill()
{
	BuyResourceUpgrade(DRILL);
}

void ADwarfPlayerState::UpgradeBoom()
{
	BuyResourceUpgrade(BOOM);
}

void ADwarfPlayerState::SetUpgradeMult1()
{
	maxMultiplier = false;
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		upgradeMultiplier[i] = 1;
		resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
	}
	FSlateColor Default = FSlateColor(FLinearColor(1, 1, 1, 1));
	FSlateColor Highlight = FSlateColor(FLinearColor(1, 1, 0, 1));
	Cast<UTextBlock>(HUD->ButtonMult1->GetChildAt(0))->SetColorAndOpacity(Highlight);
	Cast<UTextBlock>(HUD->ButtonMult5->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult10->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult25->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult100->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMultMax->GetChildAt(0))->SetColorAndOpacity(Default);
}

void ADwarfPlayerState::SetUpgradeMult5()
{
	maxMultiplier = false;
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		upgradeMultiplier[i] = 5;
		resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
	}
	FSlateColor Default = FSlateColor(FLinearColor(1, 1, 1, 1));
	FSlateColor Highlight = FSlateColor(FLinearColor(1, 1, 0, 1));
	Cast<UTextBlock>(HUD->ButtonMult1->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult5->GetChildAt(0))->SetColorAndOpacity(Highlight);
	Cast<UTextBlock>(HUD->ButtonMult10->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult25->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult100->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMultMax->GetChildAt(0))->SetColorAndOpacity(Default);
}

void ADwarfPlayerState::SetUpgradeMult10()
{
	maxMultiplier = false;
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		upgradeMultiplier[i] = 10;
		resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
	}

	FSlateColor Default = FSlateColor(FLinearColor(1, 1, 1, 1));
	FSlateColor Highlight = FSlateColor(FLinearColor(1, 1, 0, 1));
	Cast<UTextBlock>(HUD->ButtonMult1->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult5->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult10->GetChildAt(0))->SetColorAndOpacity(Highlight);
	Cast<UTextBlock>(HUD->ButtonMult25->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult100->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMultMax->GetChildAt(0))->SetColorAndOpacity(Default);
}

void ADwarfPlayerState::SetUpgradeMult25()
{
	maxMultiplier = false;
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		upgradeMultiplier[i] = 25;
		resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
	}
	FSlateColor Default = FSlateColor(FLinearColor(1, 1, 1, 1));
	FSlateColor Highlight = FSlateColor(FLinearColor(1, 1, 0, 1));
	Cast<UTextBlock>(HUD->ButtonMult1->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult5->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult10->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult25->GetChildAt(0))->SetColorAndOpacity(Highlight);
	Cast<UTextBlock>(HUD->ButtonMult100->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMultMax->GetChildAt(0))->SetColorAndOpacity(Default);
}

void ADwarfPlayerState::SetUpgradeMult100()
{
	maxMultiplier = false;
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		upgradeMultiplier[i] = 100;
		resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
	}
	FSlateColor Default = FSlateColor(FLinearColor(1, 1, 1, 1));
	FSlateColor Highlight = FSlateColor(FLinearColor(1, 1, 0, 1));
	Cast<UTextBlock>(HUD->ButtonMult1->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult5->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult10->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult25->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult100->GetChildAt(0))->SetColorAndOpacity(Highlight);
	Cast<UTextBlock>(HUD->ButtonMultMax->GetChildAt(0))->SetColorAndOpacity(Default);
}

void ADwarfPlayerState::SetUpgradeMultMax()
{
	maxMultiplier = true;

	FSlateColor Default = FSlateColor(FLinearColor(1, 1, 1, 1));
	FSlateColor Highlight = FSlateColor(FLinearColor(1, 1, 0, 1));
	Cast<UTextBlock>(HUD->ButtonMult1->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult5->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult10->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult25->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMult100->GetChildAt(0))->SetColorAndOpacity(Default);
	Cast<UTextBlock>(HUD->ButtonMultMax->GetChildAt(0))->SetColorAndOpacity(Highlight);
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
	//UpgradeItems[STRONG_ARMS]->SetDamageText(GetUpgradeDamageText(STRONG_ARMS));
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

void ADwarfPlayerState::IncreaseRogueExp(int _value)
{
	rogueData.experience += _value;

	while (rogueData.experience > rogueData.experienceRequired)
	{
		rogueData.LevelUpRogue();

		RogueHUD->LevelText->SetText(FText::FromString("Rogue Level: " + FString::FromInt(rogueData.level)));
	}
	RogueHUD->LevelProgress->SetCompletion((float)rogueData.experience / (float)rogueData.experienceRequired);
}

FString ADwarfPlayerState::GetUpgradeDamageText(UpgradeType _upgrade)
{
	switch (_upgrade)
	{
	case STRONG_ARMS: return FString::FromInt(MinDamage) + "-" + FString::FromInt(MaxDamage);
	case DRILL: return FString::SanitizeFloat(Drill.GetDPS()) + "/s";
	case BOOM: return FString::SanitizeFloat(Boom.GetDPS()) + "/s";
	default: return "Unknown Upgrade type";
	}
}

int ADwarfPlayerState::GetClickDamage()
{
	if (inRun)
	{
		int DamageDelta = rogueData.MaxDamage - rogueData.MinDamage;
		int DamageBonus = rand() % (DamageDelta + 1);
		
		// Damage Items //
		int damage = rogueData.MinDamage + DamageBonus;
		rogueData.OnDamageCalc.Broadcast(damage);

		return damage;
	}

	int DamageDelta = MaxDamage - MinDamage;
	int DamageBonus = rand() % (DamageDelta + 1);
	return MinDamage + DamageBonus;
}

void ADwarfPlayerState::Hit()
{
	int damage = GetClickDamage();
	Damage(damage,  ClickSource, currentCave);
}

void ADwarfPlayerState::Damage(int _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	if(_cave == currentCave) // Only show text if the cave damaged is displayed
	{
		if (_source.TextType == ClickSource.TextType && _damage == MaxDamage)
		{
			CreateDamageText(_damage, CRITICAL);
		}
		else
		{
			CreateDamageText(_damage, _source.TextType);
		}
	}

	_cave->DamageFirst(_damage);
}

void ADwarfPlayerState::DamageIdleCave(int _damage, DamageSource _source)
{
	BlockData currentBlockData = idleCave.first->Data;
	_damage *= _source.BlockDamageMultiplier[currentBlockData.type];

	if (&idleCave == currentCave) // Only show text if the cave damaged is displayed
	{
		if (_source.TextType == ClickSource.TextType && _damage == MaxDamage)
		{
			CreateDamageText(_damage, CRITICAL);
		}
		else
		{
			CreateDamageText(_damage, _source.TextType);
		}
	}

	idleCave.DamageFirst(_damage);
}

void ADwarfPlayerState::BlockRewardIdle(BlockData _data)
{
	IncreaseBlocks();

	//DEBUG PURPOSES ONLY
	IncreaseExp(1);

	for (auto currentYield : _data.yield)
	{
		resources[currentYield.Type] += currentYield.Amount * GlobalYieldMultiplier * resourceYieldMultiplier[currentYield.Type];
	}

	HUD->UpdateResources(resources);
	resourcesDirty = true;
}

void ADwarfPlayerState::BlockRewardRogue(BlockData _data)
{
	IncreaseRogueExp(_data.expValue);
}

void ADwarfPlayerState::FocusIdle()
{
	CameraActor->SetState(ADwarfCameraActor::IDLE);
	MainMenu->SetVisibility(ESlateVisibility::Hidden);
	HUD->SetVisibility(ESlateVisibility::Visible);

	CameraActor->pawn = currentCave->dwarfPawn;
}

void ADwarfPlayerState::FocusRogue()
{
	CameraActor->SetState(ADwarfCameraActor::IDLE);
	MainMenu->SetVisibility(ESlateVisibility::Hidden);
	RogueHUD->SetVisibility(ESlateVisibility::Visible);

	CameraActor->pawn = currentCave->dwarfPawn;
}

void ADwarfPlayerState::FocusMenu()
{
	CameraActor->SetState(ADwarfCameraActor::MENU);
	HUD->SetVisibility(ESlateVisibility::Hidden);
	MainMenu->SetVisibility(ESlateVisibility::Visible);

	CameraActor->pawn = currentCave->dwarfPawn;
}

void ADwarfPlayerState::StartRun()
{
	inRun = true;
	rogueData = RoguePlayerData();
	DamageRogueItem* debugItem = new DamageRogueItem();
	debugItem->Bind(&rogueData);
	debugItem->Bind(&rogueData);
	debugItem->Bind(&rogueData);

	// Hide idle cave
	idleCave.SetCaveVisible(false);

	// Create new rogue cave
	currentCave = new RogueCave(); // TODO: Create once and reset between runs

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	currentCave->dwarfPawn = GetWorld()->SpawnActor<ADwarfPawn>(DwarfPawnClass, FVector(), FRotator(), param);
	if (!currentCave->dwarfPawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Cannot instanciate pawn");
		return;
	}

	currentCave->BlockBreakDelegate.BindUObject(this, &ADwarfPlayerState::BlockRewardRogue);
	currentCave->GenerateStart();
	currentCave->dwarfPawn->PositionToCave(currentCave);

	RogueHUD->LevelText->SetText(FText::FromString("Rogue Level: " + FString::FromInt(rogueData.level)));
	RogueHUD->LevelProgress->SetCompletion((float)rogueData.experience / (float)rogueData.experienceRequired);

	FocusRogue();
}

void ADwarfPlayerState::EndRun()
{
	if (currentCave)
	{
		currentCave->dwarfPawn->Destroy();
		currentCave->DestroyCave();
	}

	inRun = false;
	idleCave.SetCaveVisible(true);
	currentCave = &idleCave;

	RogueHUD->SetVisibility(ESlateVisibility::Hidden);
	FocusMenu();
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

void ADwarfPlayerState::UpdateDamager(AutomaticDamager& _damager, float _dt)
{
	if (_damager.active)
	{
		_damager.Clock -= _dt;
		_damager.Display->ProgressBar->SetCompletion((_damager.Downtime - _damager.Clock) / _damager.Downtime);
		while (_damager.IsHitting())
		{
			Damage(_damager.Damage, _damager.source, &idleCave);
		}
	}
}

void ADwarfPlayerState::CreateDamageText(int _damage, DamageTextType _type)
{
	if (!DamageTextClass) return;

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

bool AutomaticDamager::IsHitting()
{
	bool isHitting = Clock <= 0;

	if (isHitting)
	{
		Clock += Downtime;
	}

	return isHitting;
}

void AutomaticDamager::SetDamagerActive(bool _active)
{
	if (_active)
	{
		Display->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Display->SetVisibility(ESlateVisibility::Hidden);
	}
	active = _active;
}

void RoguePlayerData::LevelUpRogue()
{
	level++;
	experience -= experienceRequired;
	experienceRequired *= 1.4f;
}
