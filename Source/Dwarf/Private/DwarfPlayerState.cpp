// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfPlayerState.h"
#include "RogueCave.h"
#include "Components/TextRenderComponent.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"
#include "DwarfSaveGame.h"

#define MAX_MULTIPLIER_VALUE 10000

uint64 MagicFunction(const unsigned int _level, const uint64 _baseValue, const float _s1, const float _s2)
{
	return uint64(_baseValue * powf((_level + _s2) / _s2, _s2 * _s1));
}

float MagicFunctionf(const unsigned int _level, const float _baseValue, const float _s1, const float _s2)
{
	return _baseValue * powf((_level + _s2) / _s2, _s2 * _s1);
}

TArray<ResourceData> CostStrongArms(unsigned int _level)
{
	const uint64 baseValueMudrock = 2;
	const float s1 = 0.4f;
	const float s2 = 20;
	TArray<ResourceData> cost;
	cost.Add({ MUDROCK, MagicFunction(_level, baseValueMudrock, s1, s2) });
	return cost;
}

TArray<ResourceData> CostDrill(unsigned int _level)
{
	const uint64 baseValue = 3;
	const float s1 = 0.4f;
	const float s2 = 15;
	TArray<ResourceData> cost;
	cost.Add({ COAL, MagicFunction(_level, baseValue, s1, s2) });
	cost.Add({ COPPER, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

TArray<ResourceData> CostBoom(unsigned int _level)
{
	const uint64 baseValue = 25;
	const float s1 = 0.45f;
	const float s2 = 14;
	TArray<ResourceData> cost;
	cost.Add({ SULFUR, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}
TArray<ResourceData> CostLaser(unsigned int _level)
{
	const uint64 baseValue = 5;
	const float s1 = 0.45f;
	const float s2 = 14;
	TArray<ResourceData> cost;
	//cost.Add({ DIAMOND, MagicFunction(_level, baseValue, s1, s2) });
	cost.Add({ MUDROCK, 1 });
	return cost;
}
TArray<ResourceData> CostEarthquake(unsigned int _level)
{
	const uint64 baseValue = 15;
	const float s1 = 0.45f;
	const float s2 = 15;
	TArray<ResourceData> cost;
	//cost.Add({ IRON, MagicFunction(_level, baseValue, s1, s2) });
	cost.Add({ MUDROCK, 1 });
	return cost;
}
TArray<ResourceData> CostCrit(unsigned int _level)
{
	const uint64 baseValue = 20;
	const float s1 = 0.40f;
	const float s2 = 15;
	TArray<ResourceData> cost;
	cost.Add({ TIN, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}
TArray<ResourceData> CostYield(unsigned int _level)
{
	const uint64 baseValue = 10;
	const float s1 = 0.40f;
	const float s2 = 10;
	TArray<ResourceData> cost;
	cost.Add({ SILVER, MagicFunction(_level, baseValue, s1, s2) });
	return cost;
}

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

	StrongArmsIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/StrongArms.StrongArms"));
	//PrecisionIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));
	//GoldLoverIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));
	//PlaceholderIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));
	//DrillIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));
	//TNTIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));
	//EarthquakeIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));
	//LaserIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Materials/Mudrock.Mudrock"));

}

void ADwarfPlayerState::SetupResourceUpgrades()
{
	resourceUpgrades[0].upgradeFunctionName = "StrongArms";
	resourceUpgrades[0].displayName = "Strong Arms";
	resourceUpgrades[0].description = "Increases click damage by 2.";
	resourceUpgrades[0].widget = HUD->ClickUpgradeBox;
	resourceUpgrades[0].widget->Icon->SetBrushFromTexture(StrongArmsIcon);
	SetupResourceUpgradeDelegate(resourceUpgrades[0], &CostStrongArms);

	resourceUpgrades[1].upgradeFunctionName = "Drill";
	resourceUpgrades[1].displayName = "Drill";
	resourceUpgrades[1].description = "An automatic drill that will hit for you.";
	resourceUpgrades[1].widget = HUD->DrillUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[1], &CostDrill);

	resourceUpgrades[2].upgradeFunctionName = "Boom";
	resourceUpgrades[2].displayName = "TNT";
	resourceUpgrades[2].description = "A slow automatic hit that deals heavy damage to the first column of blocks.";
	resourceUpgrades[2].widget = HUD->BoomUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[2], &CostBoom);

	resourceUpgrades[3].upgradeFunctionName = "Earthquake";
	resourceUpgrades[3].displayName = "Earthquake Totem";
	resourceUpgrades[3].description = "Shakes up the ground in front of you to clear any rubble standing in your way.";
	resourceUpgrades[3].widget = HUD->EarthquakeUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[3], &CostEarthquake);

	resourceUpgrades[4].upgradeFunctionName = "Laser";
	resourceUpgrades[4].displayName = "Mining Laser";
	resourceUpgrades[4].description = "Constantly piercing through in front of it.";
	resourceUpgrades[4].widget = HUD->LaserUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[4], &CostLaser);

	resourceUpgrades[5].upgradeFunctionName = "Crit";
	resourceUpgrades[5].displayName = "Precision";
	resourceUpgrades[5].description = "Gives you 10% chance to deal 5x damage on click.";
	resourceUpgrades[5].widget = HUD->CritUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[5], &CostCrit);

	resourceUpgrades[6].upgradeFunctionName = "Yield";
	resourceUpgrades[6].displayName = "Gold Lover";
	resourceUpgrades[6].description = "Increases your global yield by 10%"; 
	resourceUpgrades[6].widget = HUD->YieldUpgradeBox;
	SetupResourceUpgradeDelegate(resourceUpgrades[6], &CostYield);
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
	}
}

void ADwarfPlayerState::SetupResourceUpgradeDelegate(ResourceUpgrade& upgrade, TArray<ResourceData> (*InFunc)(unsigned int))
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
	Drill.damageType = FIRST;

	Drill.Display = HUD->DrillDisplay;
	Drill.Display->ProgressBar->SetCompletion((Drill.Downtime - Drill.Clock) / Drill.Downtime);
	Drill.Display->Name = "Drill";
	Drill.Display->Description = "Hits the next block for you.";
	Drill.Display->damage = Drill.Damage;
	Drill.Display->hitCooldown = Drill.Downtime;

	Drill.SetDamagerActive(false);
	Drill.source.TextType = AUTO;

	Boom.Damage = 0;
	Boom.Downtime = 10.0f;
	Boom.Clock = Boom.Downtime;
	Boom.damageType = COLUMN;

	Boom.Display = HUD->BoomDisplay;
	Boom.Display->ProgressBar->SetCompletion((Boom.Downtime - Boom.Clock) / Boom.Downtime);
	Boom.Display->Name = "TNT";
	Boom.Display->Description = "It's explosion heavily damages the nearest column of blocks.";
	Boom.Display->damage = Boom.Damage;
	Boom.Display->hitCooldown = Boom.Downtime;

	Boom.SetDamagerActive(false);
	Boom.source.TextType = AUTO;

	EarthquakeTotem.Damage = 0;
	EarthquakeTotem.Downtime = 5.0f;
	EarthquakeTotem.Clock = EarthquakeTotem.Downtime;
	EarthquakeTotem.damageType = AREA;

	EarthquakeTotem.Display = HUD->EarthquakeDisplay;
	EarthquakeTotem.Display->ProgressBar->SetCompletion((EarthquakeTotem.Downtime - EarthquakeTotem.Clock) / EarthquakeTotem.Downtime);
	EarthquakeTotem.Display->Name = "Earthquake Totem";
	EarthquakeTotem.Display->Description = "Shakes up the ground in front of you to clear any rubble standing in your way.";
	EarthquakeTotem.Display->damage = EarthquakeTotem.Damage;
	EarthquakeTotem.Display->hitCooldown = EarthquakeTotem.Downtime;

	EarthquakeTotem.SetDamagerActive(false);
	EarthquakeTotem.source.TextType = AUTO;

	Laser.Damage = 0;
	Laser.Downtime = 0.5f;
	Laser.Clock = Laser.Downtime;
	Laser.damageType = LINE;

	Laser.Display = HUD->LaserDisplay;
	Laser.Display->ProgressBar->SetCompletion((Laser.Downtime - Laser.Clock) / Laser.Downtime);
	Laser.Display->Name = "Mining Laser";
	Laser.Display->Description = "Constantly piercing through in front of it.";
	Laser.Display->damage = Laser.Damage;
	Laser.Display->hitCooldown = Laser.Downtime;

	Laser.SetDamagerActive(false);
	Laser.source.TextType = AUTO;
}

void ADwarfPlayerState::InitRelicsArray()
{
	relics.Add(new DamageIdleRelic());
	relics.Add(new MultihitIdleRelic());
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

	if (TimeBoost)
	{
		TimeBoostLeft -= _dt;
		if (TimeBoostLeft <= 0)
		{
			currentCave->dwarfPawn->TimeFactor = 1.0f;
			TimeBoost = false;
		}
		_dt *= TimeBoostFactor;
	}

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
		FString costString = "Cost (x" + FString::FromInt(upgradeMultiplier[i]) + "): \n";
		for (auto resource : resourceUpgrades[i].costCached)
		{
			bool resourceAvailable = true;

			costString += GetResourceIcon(resource.Type);
			if (resource.Amount > resources[resource.Type])
			{
				resourceAvailable = false;
				upgradeAvailable = false;
				costString += "<UnavailableEmphasis>: " + resource.Amount.ToString() + "</>\n";
			}
			else
			{
				costString += ": " + resource.Amount.ToString() + "\n";
			}
		}

		FLinearColor availabilityTint = upgradeAvailable ? FLinearColor(1, 1, 1, 1) : FLinearColor(0.1, 0.1, 0.1, 1);
		resourceUpgrades[i].widget->SetColorAndOpacity(availabilityTint);
		resourceUpgrades[i].widget->costText = costString;
	}

	// Idle auto damagers
	UpdateDamager(Drill, _dt);
	UpdateDamager(Boom, _dt);
	UpdateDamager(EarthquakeTotem, _dt);
	UpdateDamager(Laser, _dt);

	if (inRun == false) return;
	if (rogueData.timePaused) return;
	// Take pressure damage
	rogueData.Pressure -= currentCave->first->Data.pressureValue * _dt * (1.0f - rogueData.PressureResistance);
	if (rogueData.Pressure <= 0) // Lose
	{
		RunRewards();
		return;
	}

	RogueHUD->PressureBar->SetPercent(rogueData.Pressure / rogueData.MaxPressure);

	// Run auto damagers
	rogueData.clickTimer -= _dt;
	if (rogueData.clickTimer <= 0)
	{
		currentCave->dwarfPawn->HitAnimation();
		rogueData.clickTimer = rogueData.GetHitCooldown();
		Damage(GetRogueClickDamage(), ClickSource, currentCave);
	}

	rogueData.OnTick.Broadcast(_dt);
}

void ADwarfPlayerState::StartGame()
{
	if (TooltipClass)
	{
		UUserWidget* tooltip = CreateWidget<UUserWidget>(GetPlayerController(), TooltipClass);
		tooltip->SetVisibility(ESlateVisibility::Hidden);
		tooltip->AddToViewport(1);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create Tooltip")));
	}

	if (RogueHUDClass)
	{
		RogueHUD = CreateWidget<URogueHUD>(GetPlayerController(), RogueHUDClass);

		RogueHUD->MenuButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::RunRewards);

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

	if (CardSelectionClass)
	{
		CardSelection = CreateWidget<URogueCardSelection>(GetPlayerController(), CardSelectionClass);

		CardSelection->CardLeft->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::CardSelectLeft);
		CardSelection->CardMiddle->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::CardSelectMiddle);
		CardSelection->CardRight->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::CardSelectRight);

		CardSelection->SetVisibility(ESlateVisibility::Hidden);
		CardSelection->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("!!! Could not create CardSelection")));
	}
	if (RelicSelectionClass)
	{
		RelicSelection = CreateWidget<URogueCardSelection>(GetPlayerController(), RelicSelectionClass);

		RelicSelection->CardLeft->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::RelicCardSelectLeft);
		RelicSelection->CardMiddle->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::RelicCardSelectMiddle);
		RelicSelection->CardRight->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::RelicCardSelectRight);

		RelicSelection->SetVisibility(ESlateVisibility::Hidden);
		RelicSelection->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("!!! Could not create RelicCardSelection")));
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
		CharacterMenu->DifficultySlider->OnValueChanged.AddDynamic(this, &ADwarfPlayerState::ChangeDifficulty);
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
	InitRelicsArray();

	ClickSource.TextType = NORMAL;

	idleCave.dwarfPawn = idlePawn;
	idleCave.BlockBreakDelegate.BindUObject(this, &ADwarfPlayerState::BlockRewardIdle);
	idleCave.DamageTextClass = DamageTextClass;
	currentCave = &idleCave;

	CameraActor = GetWorld()->SpawnActor<ADwarfCameraActor>(ADwarfCameraActor::StaticClass(), FVector(), FRotator(), FActorSpawnParameters());
	CameraActor->pawn = currentCave->dwarfPawn;
	CameraActor->ForcePos();

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
		SetupResourceUpgrades();
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString(TEXT("First time launching the game.")));
		return;
	}

	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT; upgrade = (UpgradeType)(1 + upgrade))
	{
		resourceUpgrades[upgrade].upgradeLevel = save->upgradeLevels[upgrade];			// Get level
		for (unsigned int i = 1; i <= resourceUpgrades[upgrade].upgradeLevel; i++)		// Re-apply upgrade
		{
			ApplyResourceUpgrade(upgrade, i);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::FromInt(i));
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(resourceUpgrades[upgrade].upgradeLevel));
	}

	SetupResourceUpgrades();

	// Resources
	memcpy(resources, save->resources, sizeof(int) * RESOURCE_COUNT);
	memcpy(isResourceUnlocked, save->isResourceUnlocked, sizeof(bool) * RESOURCE_COUNT);
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		if (isResourceUnlocked[i]) HUD->SetResource(i, resources[i]);
	}
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

	Level = save->level;
	Experience = save->exp;

	for (int i = 0; i < RELIC_COUNT; i++)
	{
		int count = save->relicCounts[relics[i]->tag];
		if (count == 0) continue;
		
		AddRelic(relics[i], count);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Relic count: " + FString::FromInt(count));
	}

	idleCave.blocksGenerated = save->blockBroken;
	idleCave.blocksBroken = save->blockBroken;
	while (idleCave.CheckRank());
	idleCave.GenerateStart();
	HUD->DistanceTraveledText->SetText(FText::FromString("Distance Traveled: " + FString::FromInt(currentCave->first->Data.index / 4) + "m"));

	// Get time since last connection
	unsigned long timeSince = difftime(time(nullptr), save->saveTime);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Time since last connection: " + FString::FromInt(timeSince) + "s");
	//Tick(timeSince); // IS THIS EVEN ALLOWED??? TODO: see if this should be the way to catchup or not
	TimeBoost = true;
	TimeBoostFactor = fmin(timeSince, 1000.0f);
	TimeBoostLeft = timeSince / TimeBoostFactor;
	currentCave->dwarfPawn->TimeFactor = TimeBoostFactor;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString(TEXT("Finished loading!")));
}

void ADwarfPlayerState::ResetDwarfStats()
{
	MinDamage = 1;
	MaxDamage = 1;
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
	
	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT; upgrade = (UpgradeType)(upgrade + 1))
	{
		save->upgradeLevels[upgrade] = resourceUpgrades[upgrade].upgradeLevel;	// Set level
	}
	memcpy(save->resources, resources, sizeof(int) * RESOURCE_COUNT);
	memcpy(save->isResourceUnlocked, isResourceUnlocked, sizeof(bool) * RESOURCE_COUNT);

	save->savedStats[0] = savedStats.blocksBroken.value;
	save->savedStats[1] = savedStats.damageDone.value;
	save->savedStats[2] = savedStats.metersWalked.value;
	save->savedStats[3] = savedStats.rebirthCount.value;
	// Experience
	save->level = Level;
	save->exp = Experience;

	int relicNum = obtainedRelics.Num();
	for (int i = 0; i < relicNum; i++)
	{
		save->relicCounts[relics[i]->tag] = obtainedRelics[i]->cumulativeCount;
	}
	
	save->blockBroken = idleCave.blocksBroken;

	// Time
	save->saveTime = time(nullptr);

	UGameplayStatics::AsyncSaveGameToSlot(save, "SaveSlot", 0, SavedDelegate);
}

void ADwarfPlayerState::SaveCurrentState()
{
	UDwarfSaveGame* save = (UDwarfSaveGame*)UGameplayStatics::CreateSaveGameObject(UDwarfSaveGame::StaticClass());
	
	//// Set all values ////

	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT; upgrade = (UpgradeType)(upgrade + 1))
	{
		save->upgradeLevels[upgrade] = resourceUpgrades[upgrade].upgradeLevel;	// Set level
	}
	memcpy(save->resources, resources, sizeof(int) * RESOURCE_COUNT);
	memcpy(save->isResourceUnlocked, isResourceUnlocked, sizeof(bool) * RESOURCE_COUNT);

	save->savedStats[0] = savedStats.blocksBroken.value;
	save->savedStats[1] = savedStats.damageDone.value;
	save->savedStats[2] = savedStats.metersWalked.value;
	save->savedStats[3] = savedStats.rebirthCount.value;

	save->level = Level;
	save->exp = Experience;
	
	int relicNum = obtainedRelics.Num();
	for (int i = 0; i < relicNum; i++)
	{
		save->relicCounts[relics[i]->tag] = obtainedRelics[i]->cumulativeCount;
	}

	save->blockBroken = idleCave.blocksBroken;

	save->saveTime = time(nullptr);

	UGameplayStatics::SaveGameToSlot(save, "SaveSlot", 0);
}

void ADwarfPlayerState::Rebirth()
{
	for (int i = 0; i < UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i].upgradeLevel = 0;
		resourceUpgrades[i].UpdateTooltipText();
		resourceUpgrades[i].GetCostAndCache(upgradeMultiplier[i]);
	}
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		resources[i] = 0;
		isResourceUnlocked[i] = false;
	}
	HUD->Populate();

	ResetDwarfStats();
	currentCave->difficultyLevel = difficulty;
	currentCave->ResetCave();
	HUD->DistanceTraveledText->SetText(FText::FromString("Distance Traveled: " + FString::FromInt(currentCave->first->Data.index / 4) + "m"));
	idlePawn->ResetDwarfPawn();
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
		HUD->SetResource(res.Type, resources[res.Type]);
	}

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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::FromInt(upgradeMultiplier[_upgrade]));
	for (int i = 0; i < upgradeMultiplier[_upgrade]; i++)
	{
		resourceUpgrades[_upgrade].upgradeLevel++;
		ApplyResourceUpgrade(_upgrade, resourceUpgrades[_upgrade].upgradeLevel);
	}
	
	resourceUpgrades[_upgrade].UpdateTooltipText();
	resourceUpgrades[_upgrade].GetCostAndCache(upgradeMultiplier[_upgrade]);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Bought upgrade " + FString::FromInt(_upgrade));
}

void ADwarfPlayerState::ApplyResourceUpgrade(UpgradeType _upgrade, unsigned int _level)
{
	switch (_upgrade)
	{
	case STRONG_ARMS:
	{
		MinDamage = MagicFunction(_level-1, 2, 0.4, 5);
		break;
	}
	case PRECISION:
	{
		CritMultiplier = MagicFunctionf(_level - 1, 2, 0.2, 10);
		switch (_level)
		{
		case 1:
			CritChance = 5;
			break;
		}
		
		break;
	}
	case GOLD_LOVER:
	{
		GlobalYieldMultiplier = 1 + MagicFunctionf(_level-1, 0.1f, 0.4f, 5);
		break;
	}
	case DRILL:
	{
		Drill.Damage = MagicFunction(_level-1, 10, 0.2, 10);
		Drill.Damage *= Drill.MilestoneDamageMult;
		switch (_level)
		{
		case 1:
			Drill.SetDamagerActive(true);
			break;
		case 10:
			Drill.MilestoneDamageMult *= 2;
			Drill.Downtime *= 0.8;
			break;
		case 25:
			Drill.MilestoneDamageMult *= 3;
			Drill.Downtime *= 0.8;
			break;
		case 50:
			Drill.MilestoneDamageMult *= 5;
			Drill.Downtime *= 0.8;
			break;
		}

		Drill.UpdateDisplayTooltip(_level);
		break;
	}
	case BOOM:
	{
		Boom.Damage = MagicFunction(_level-1, 50, 0.25, 10);
		Boom.Damage *= Boom.MilestoneDamageMult;
		switch (_level)
		{
		case 1:
			Boom.SetDamagerActive(true);
			break;
		case 10:
			Boom.MilestoneDamageMult *= 2;
			Boom.Downtime *= 0.8;
			break;
		case 25:
			Boom.MilestoneDamageMult *= 3;
			Boom.Downtime *= 0.8;
			break;
		case 50:
			Boom.MilestoneDamageMult *= 5;
			Boom.Downtime *= 0.8;
			break;
		}

		Boom.UpdateDisplayTooltip(_level);
		break;
	}
	case EARTHQUAKE:
	{
		EarthquakeTotem.Damage = MagicFunction(_level-1, 10, 0.3, 10);
		EarthquakeTotem.Damage *= EarthquakeTotem.MilestoneDamageMult;
		switch (_level)
		{
		case 1:
			EarthquakeTotem.SetDamagerActive(true);
			break;
		case 10:
			EarthquakeTotem.MilestoneDamageMult *= 2;
			EarthquakeTotem.Downtime *= 0.8;
			break;
		case 25:
			EarthquakeTotem.MilestoneDamageMult *= 3;
			EarthquakeTotem.Downtime *= 0.8;
			break;
		case 50:
			EarthquakeTotem.MilestoneDamageMult *= 5;
			EarthquakeTotem.Downtime *= 0.8;
			break;
		}

		EarthquakeTotem.UpdateDisplayTooltip(_level);
		break;
	}
	case LASER:
	{
		Laser.Damage = MagicFunction(_level-1, 5, 0.4, 10);
		Laser.Damage *= Laser.MilestoneDamageMult;
		switch (_level)
		{
		case 1:
			Laser.SetDamagerActive(true);
			break;
		case 10:
			Laser.MilestoneDamageMult *= 2;
			Laser.Downtime *= 0.8;
			break;
		case 25:
			Laser.MilestoneDamageMult *= 3;
			Laser.Downtime *= 0.8;
			break;
		case 50:
			Laser.MilestoneDamageMult *= 5;
			Laser.Downtime *= 0.8;
			break;
		}

		Laser.UpdateDisplayTooltip(_level);
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
		string += resource.Amount.ToString();
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

void ADwarfPlayerState::CardSelectLeft()
{
	CardSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData.timePaused = false;
	AddItemFromPool(itemsSelection[0]);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Left selected")));
}

void ADwarfPlayerState::CardSelectMiddle()
{
	CardSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData.timePaused = false;
	AddItemFromPool(itemsSelection[1]);
}

void ADwarfPlayerState::CardSelectRight()
{
	CardSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData.timePaused = false;
	AddItemFromPool(itemsSelection[2]);
}

void ADwarfPlayerState::RelicCardSelectLeft()
{
	RelicSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData.timePaused = false;
	AddRelic(relicCardSelection[0]);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Left selected")));
	EndRun();
}

void ADwarfPlayerState::RelicCardSelectMiddle()
{
	RelicSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData.timePaused = false;
	AddRelic(relicCardSelection[1]);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Middle selected")));
	EndRun();
}

void ADwarfPlayerState::RelicCardSelectRight()
{
	RelicSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData.timePaused = false;
	AddRelic(relicCardSelection[2]);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Right selected")));
	EndRun();
}

void ADwarfPlayerState::AddRelic(IdleRelic* _relic)
{
	int relicIndex = obtainedRelics.Find(_relic);
	if (relicIndex == INDEX_NONE) // New relic
	{
		_relic->IncreaseCount(1);

		obtainedRelics.Add(_relic);
		_relic->Bind(this);
		// Add to box
		UUpgradeEntryWidget* widget = CreateWidget<UUpgradeEntryWidget, UWrapBox*>(HUD->RelicBox, ItemBoxClass);
		widget->level = _relic->rank;
		widget->name = _relic->name;
		widget->SetPadding(16);
		widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
		widget->descriptionText = _relic->GetDescriptionText(_relic->rank);
		HUD->RelicBox->AddChild(widget);
		_relic->widget = widget;
	}

	else
	{
		_relic->IncreaseCount(1);
		_relic->widget->descriptionText = _relic->GetDescriptionText(_relic->rank);
		_relic->widget->level = _relic->rank;
	}
}

void ADwarfPlayerState::AddRelic(IdleRelic* _relic, int _count)
{
	_relic->IncreaseCount(_count);

	obtainedRelics.Add(_relic);
	_relic->Bind(this);
	// Add to box
	UUpgradeEntryWidget* widget = CreateWidget<UUpgradeEntryWidget, UWrapBox*>(HUD->RelicBox, ItemBoxClass);
	widget->level = 1;
	widget->name = _relic->name;
	widget->SetPadding(16);
	widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
	widget->descriptionText = _relic->GetDescriptionText(_relic->rank);
	HUD->RelicBox->AddChild(widget);
}

void ADwarfPlayerState::AddItem(RogueItem* _item)
{
	// Check if Item already exists
	int itemIndex = rogueData.items.Find(_item);
	if (itemIndex != INDEX_NONE)
	{ 
		rogueData.items[itemIndex]->level++;

		UUpgradeEntryWidget* widget = rogueData.itemWidgets[itemIndex];
		widget->level++;
		widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
		widget->descriptionText = _item->GetDescriptionText(_item->level);
	}

	else
	{
		// New item: Bind and add widget
		_item->Bind(&rogueData, currentCave);
		_item->level = 1;

		UUpgradeEntryWidget* widget = CreateWidget<UUpgradeEntryWidget, UWrapBox*>(RogueHUD->ItemBox, ItemBoxClass);
		widget->level = 1;
		widget->name = _item->name;
		widget->SetPadding(16);
		widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
		widget->descriptionText = _item->GetDescriptionText(_item->level);
		RogueHUD->ItemBox->AddChild(widget);

		rogueData.itemWidgets.Add(widget);
	}
}

void ADwarfPlayerState::AddItemFromPool(unsigned int _index)
{
	AddItem(currentItemPool[_index]);
	if (currentItemPool[_index]->level >= MAX_ITEM_LEVEL)
	{
		currentItemPool.RemoveAt(_index);
	}
}

void ADwarfPlayerState::UpgradeStrongArms()
{
	BuyResourceUpgrade(STRONG_ARMS);
}

void ADwarfPlayerState::UpgradeDrill()
{
	BuyResourceUpgrade(DRILL);
}
void ADwarfPlayerState::UpgradeEarthquake()
{
	BuyResourceUpgrade(EARTHQUAKE);
}
void ADwarfPlayerState::UpgradeLaser()
{
	BuyResourceUpgrade(LASER);
}

void ADwarfPlayerState::UpgradeCrit()
{
	BuyResourceUpgrade(PRECISION);
}

void ADwarfPlayerState::UpgradeYield()
{
	BuyResourceUpgrade(GOLD_LOVER);
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
	return;
	//// Check milestones:
	//switch (_tier)
	//{
	//case 0:
	//{
	//	MinDamage += 5;
	//	MaxDamage += 5;
	//	break;
	//}
	//case 1:
	//{
	//	MinDamage += 10;
	//	MaxDamage += 10;
	//	break;
	//}
	//case 2:
	//{
	//	MinDamage += 15;
	//	MaxDamage += 15;
	//	break;
	//}
	//case 3:
	//{
	//	MinDamage += 20;
	//	MaxDamage += 20;
	//	break;
	//}
	//}
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

void ADwarfPlayerState::LevelUpRogue()
{
	rogueData.LevelUpRogue();
	TArray<int> tempPool;
	for (int i = 0; i < currentItemPool.Num(); i++) tempPool.Add(i);

	if (currentItemPool.Num() < 3) return; // TODO: Flat stat increase

	// Item TODO: Create actual item random (with at least 1 level up, rarity weight, and max level restriction)
	int randIndex = rand() % tempPool.Num();
	itemsSelection[0] = tempPool[randIndex];
	tempPool.RemoveAt(randIndex);
	CardSelection->CardLeft->SetItemInfo(currentItemPool[itemsSelection[0]]);

	randIndex = rand() % tempPool.Num();
	itemsSelection[1] = tempPool[randIndex];
	tempPool.RemoveAt(randIndex);
	CardSelection->CardMiddle->SetItemInfo(currentItemPool[itemsSelection[1]]);

	randIndex = rand() % tempPool.Num();
	itemsSelection[2] = tempPool[randIndex];
	tempPool.RemoveAt(randIndex);
	CardSelection->CardRight->SetItemInfo(currentItemPool[itemsSelection[2]]);

	RogueHUD->LevelText->SetText(FText::FromString("Rogue Level: " + FString::FromInt(rogueData.level)));

	rogueData.timePaused = true;
	CardSelection->SetVisibility(ESlateVisibility::Visible);
}

void ADwarfPlayerState::IncreaseRogueExp(int _value)
{
	rogueData.experience += _value;

	while (rogueData.experience >= rogueData.experienceRequired)
	{
		LevelUpRogue();
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
	int finalMinDamage = MinDamage * ClickDamageMultiplier;
	MaxDamage = finalMinDamage * DamageWindow;
	int DamageDelta = MaxDamage - finalMinDamage;
	int DamageBonus = rand() % (DamageDelta + 1);
	int finalDamage = finalMinDamage + DamageBonus;

	int critChanceTemp = CritChance; // Allows for "overcrit"
	while (rand() % 100 < critChanceTemp)
	{
		finalDamage *= CritMultiplier;
		critChanceTemp -= 100;
	}

	return finalDamage;
}

int ADwarfPlayerState::GetRogueClickDamage()
{
	int DamageDelta = rogueData.MaxDamage - rogueData.MinDamage;
	int DamageBonus = rand() % (DamageDelta + 1);

	// Damage Items //
	int damage = rogueData.MinDamage + DamageBonus;
	rogueData.OnDamageCalc.Broadcast(damage);

	return damage;
}

void ADwarfPlayerState::Hit(bool _silent = false)
{
	if (inRun) return;

	if(_silent == false) OnHit.Broadcast(this);

	currentCave->dwarfPawn->HitAnimation();

	int damage = GetClickDamage();
	OnClickDamageCalc.Broadcast(damage);
	Damage(damage,  ClickSource, currentCave);
}

void ADwarfPlayerState::Damage(int _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageFirst(_damage, _source);
}

void ADwarfPlayerState::DamageColumn(int _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageFirstColumn(_damage, _source);
}

void ADwarfPlayerState::DamageArea(int _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageArea(_damage, _source);
}

void ADwarfPlayerState::DamageRow(int _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageRow(_damage, _source);
}

void ADwarfPlayerState::DamageIdleCave(int _damage, DamageSource _source)
{
	BlockData currentBlockData = idleCave.first->Data;
	_damage *= _source.BlockDamageMultiplier[currentBlockData.type];

	idleCave.DamageFirst(_damage, _source);
}

void ADwarfPlayerState::BlockRewardIdle(BlockData _data)
{
	IncreaseBlocks();

	for (auto currentYield : _data.yield)
	{
		resources[currentYield.Type] += currentYield.Amount * GlobalYieldMultiplier * resourceYieldMultiplier[currentYield.Type] * idleCave.yieldMultiplier;
		HUD->SetResource(currentYield.Type, resources[currentYield.Type]);
		isResourceUnlocked[currentYield.Type] = true;
	}

	resourcesDirty = true;

	HUD->DistanceTraveledText->SetText(FText::FromString("Distance Traveled: " + FString::FromInt(currentCave->first->Data.index / 4) + "m"));
}

void ADwarfPlayerState::BlockRewardRogue(BlockData _data)
{
	IncreaseRogueExp(GetRogueExp(_data.expValue));
	rogueData.Pressure += _data.pressureRegen;
	rogueData.Pressure = fmin(rogueData.Pressure, rogueData.MaxPressure);
	RogueHUD->PressureBar->SetPercent(rogueData.Pressure / rogueData.MaxPressure);
}

int ADwarfPlayerState::GetRogueExp(int _exp)
{
	int expValue = _exp;
	// Call delegate
	rogueData.OnExpCalc.Broadcast(expValue);
	return expValue;
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

	// Build the item pool
	unlockedItemPool.Add(new DamageRogueItem());
	unlockedItemPool[0]->associatedRelic = relics[0];
	unlockedItemPool.Add(new CooldownRogueItem());
	unlockedItemPool[1]->associatedRelic = nullptr;
	unlockedItemPool.Add(new MultihitRogueItem());
	unlockedItemPool[2]->associatedRelic = relics[1];
	currentItemPool = unlockedItemPool;

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
	currentCave->DamageTextClass = DamageTextClass;
	currentCave->GenerateStart();
	currentCave->dwarfPawn->PositionToCave(currentCave);

	RogueHUD->LevelText->SetText(FText::FromString("Rogue Level: " + FString::FromInt(rogueData.level)));
	RogueHUD->LevelProgress->SetCompletion((float)rogueData.experience / (float)rogueData.experienceRequired);

	FocusRogue();
}

void ADwarfPlayerState::RunRewards()
{
	TArray<RogueItem*> tempItems = rogueData.items;
	int totalWeight = 0;

	int itemCount = tempItems.Num();
	int relicItemCount = itemCount;
	for (int i = 0; i < itemCount; i++)
	{
		if (tempItems[i]->associatedRelic == nullptr)
		{
			relicItemCount--;
			continue;
		}
		totalWeight += tempItems[i]->GetRelicWeight();
	}

	if (relicItemCount <= 0)
	{
		EndRun();
		return;
	}

	RelicSelection->SetVisibility(ESlateVisibility::Visible);
	rogueData.timePaused = true;

	UItemCard* cards[3];
	cards[0] = RelicSelection->CardLeft;	RelicSelection->CardLeft->SetVisibility(ESlateVisibility::Hidden);
	cards[1] = RelicSelection->CardMiddle;	RelicSelection->CardMiddle->SetVisibility(ESlateVisibility::Hidden);
	cards[2] = RelicSelection->CardRight;	RelicSelection->CardRight->SetVisibility(ESlateVisibility::Hidden);

	for (int i = 0; i < 3; i++)
	{
		int relicPick = rand() % totalWeight;
		int tempWeight = 0;
		for (int j = 0; j < itemCount; j++)
		{
			if (tempItems[j]->associatedRelic == nullptr) continue;

			tempWeight += tempItems[j]->GetRelicWeight();
			if (relicPick < tempWeight)
			{
				relicCardSelection[i] = tempItems[j]->associatedRelic;
				totalWeight -= tempItems[j]->GetRelicWeight();
				tempItems.RemoveAt(j);
				break;
			}
		}
		cards[i]->SetRelicInfo(relicCardSelection[i]);
		cards[i]->SetVisibility(ESlateVisibility::Visible);
		relicItemCount--;
		if (relicItemCount <= 0) return;
	}
}

void ADwarfPlayerState::EndRun()
{
	if (currentCave)
	{
		currentCave->dwarfPawn->Destroy();
		currentCave->DestroyCave();
	}

	currentItemPool.Empty();
	rogueData.items.Empty();
	for (int i = 0; i < rogueData.itemWidgets.Num(); i++)
	{
		rogueData.itemWidgets[i]->RemoveFromParent();
	}
	rogueData.itemWidgets.Empty();

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

void ADwarfPlayerState::ChangeDifficulty(float _value)
{
	difficulty = _value;
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
			int damage = _damager.Damage;
			_damager.OnDamageCalc.Broadcast(damage);
			switch (_damager.damageType)
			{
			case FIRST:
			{
				Damage(damage, _damager.source, &idleCave);
				break;
			}
			case COLUMN:
			{
				DamageColumn(damage, _damager.source, &idleCave);
				break;
			}
			case AREA:
			{
				DamageArea(damage, _damager.source, &idleCave);
				break;
			}
			case LINE:
			{
				DamageRow(damage, _damager.source, &idleCave);
				break;
			}
			default:
			{
				Damage(damage, _damager.source, &idleCave);
				break;
			}
			}
			
		}
	}
}

bool AutomaticDamager::IsHitting()
{
	bool isHitting = Clock <= 0;

	if (isHitting)
	{
		float cooldown = Downtime;
		OnCooldownCalc.Broadcast(cooldown);
		Clock += cooldown;
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

void AutomaticDamager::UpdateDisplayTooltip(int _level)
{
	Display->damage = Damage;
	Display->hitCooldown = Downtime;
	Display->level = _level;
}

void RoguePlayerData::LevelUpRogue()
{
	level++;
	experience -= experienceRequired;
	experienceRequired *= 1.4f;
}

float RoguePlayerData::GetHitCooldown()
{
	float cd = clickCooldown;
	OnCooldownCalc.Broadcast(cd);
	return cd;
}
