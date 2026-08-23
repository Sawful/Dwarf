// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfPlayerState.h"
#include "RogueCave.h"
#include "Components/TextRenderComponent.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"
#include "DwarfSaveGame.h"
#include "RogueItemIncludes.h"
#include "MathTools.h"
#include "Engine.h"

#define MAX_MULTIPLIER_VALUE 10000


FString DescriptionCrit(unsigned int _level)
{
	"Gives you 10% chance to deal 5x damage on click.";
	return FString();
}

FString DescriptionYield(unsigned int _level)
{
	"Increases your global yield by 10%";
	return FString();
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
}

void ADwarfPlayerState::SetupResourceUpgrades()
{
	resourceUpgrades[0] = new StrongArmsUpgrade();
	resourceUpgrades[1] = new DrillUpgrade();
	resourceUpgrades[2] = new BoomUpgrade();
	resourceUpgrades[3] = new EarthquakeUpgrade();
	resourceUpgrades[4] = new LaserUpgrade();
	resourceUpgrades[5] = new CritUpgrade();
	resourceUpgrades[6] = new YieldUpgrade();
	resourceUpgrades[7] = new PlaceholderUpgrade();

	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i]->player = this;
	}

	resourceUpgrades[0]->SetWidget(HUD->ClickUpgradeBox);
	resourceUpgrades[1]->SetWidget(HUD->DrillUpgradeBox);
	resourceUpgrades[2]->SetWidget(HUD->BoomUpgradeBox);
	resourceUpgrades[3]->SetWidget(HUD->EarthquakeUpgradeBox);
	resourceUpgrades[4]->SetWidget(HUD->LaserUpgradeBox);
	resourceUpgrades[5]->SetWidget(HUD->CritUpgradeBox);
	resourceUpgrades[6]->SetWidget(HUD->YieldUpgradeBox);
	resourceUpgrades[7]->SetWidget(HUD->PlaceholderUpgradeBox);
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
	Drill.Display->DamageText = Drill.Damage.ToStringTrunc();
	Drill.Display->hitCooldown = Drill.Downtime;
	Drill.Display->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Drill.Drill")));

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
	Boom.Display->DamageText = Boom.Damage.ToStringTrunc();
	Boom.Display->hitCooldown = Boom.Downtime;
	Boom.Display->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/TNT.TNT")));

	Boom.SetDamagerActive(false);
	Boom.source.TextType = AUTO;

	Earthquake.Damage = 0;
	Earthquake.Downtime = 5.0f;
	Earthquake.Clock = Earthquake.Downtime;
	Earthquake.damageType = AREA;

	Earthquake.Display = HUD->EarthquakeDisplay;
	Earthquake.Display->ProgressBar->SetCompletion((Earthquake.Downtime - Earthquake.Clock) / Earthquake.Downtime);
	Earthquake.Display->Name = "Earthquake Totem";
	Earthquake.Display->Description = "Shakes up the ground in front of you to clear any rubble standing in your way.";
	Earthquake.Display->DamageText = Earthquake.Damage.ToStringTrunc();
	Earthquake.Display->hitCooldown = Earthquake.Downtime;
	Earthquake.Display->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Totem.Totem")));

	Earthquake.SetDamagerActive(false);
	Earthquake.source.TextType = AUTO;

	Laser.Damage = 0;
	Laser.Downtime = 0.5f;
	Laser.Clock = Laser.Downtime;
	Laser.damageType = LINE;

	Laser.Display = HUD->LaserDisplay;
	Laser.Display->ProgressBar->SetCompletion((Laser.Downtime - Laser.Clock) / Laser.Downtime);
	Laser.Display->Name = "Mining Laser";
	Laser.Display->Description = "Constantly piercing through in front of it.";
	Laser.Display->DamageText = Laser.Damage.ToStringTrunc();
	Laser.Display->hitCooldown = Laser.Downtime;
	Laser.Display->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Texture/UI/Upgrades/Laser.Laser")));

	Laser.SetDamagerActive(false);
	Laser.source.TextType = AUTO;
}

void ADwarfPlayerState::InitRelicsArray()
{
	relics.Add(new DamageIdleRelic());
	relics.Add(new MultihitIdleRelic());
	relics.Add(nullptr);

	// Autodamager upgrades
	relics.Add(new DrillIdleRelic());
	relics.Add(new BoomIdleRelic());
	relics.Add(new EarthquakeIdleRelic());
	relics.Add(new LaserIdleRelic());

	relics.Add(nullptr);
	relics.Add(nullptr);
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

	if (gameLoaded == false) return;

	// Check upgrade costs
	// If mult is Max, find "max"
	if (maxMultiplier)
	{
		for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
		{
			if (resourcesDirty)
			{
				resourceUpgrades[i]->SetBuyMultiplier(GetMaxUpgradeMult((UpgradeType)i));
			}

			resourceUpgrades[i]->GetCostAndCache();
		}
	}

	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		bool upgradeAvailable = true;
		FString costString = "Cost (x" + FString::FromInt(resourceUpgrades[i]->buyMultiplier) + "): \n";
		for (auto resource : resourceUpgrades[i]->costCached)
		{
			bool resourceAvailable = true;

			costString += GetResourceIcon(resource.Type);
			if (resource.Amount > resources[resource.Type])
			{
				resourceAvailable = false;
				upgradeAvailable = false;
				costString += "<UnavailableEmphasis>: " + resource.Amount.ToStringTrunc() + "</>\n";
			}
			else
			{
				costString += ": " + resource.Amount.ToStringTrunc() + "\n";
			}
		}

		FLinearColor availabilityTint = upgradeAvailable ? FLinearColor(1, 1, 1, 1) : FLinearColor(0.1, 0.1, 0.1, 1);
		resourceUpgrades[i]->widget->SetColorAndOpacity(availabilityTint);
		resourceUpgrades[i]->widget->costText = costString;
	}

	// Idle auto damagers
	UpdateDamager(Drill, _dt);
	UpdateDamager(Boom, _dt);
	UpdateDamager(Earthquake, _dt);
	UpdateDamager(Laser, _dt);

	if (inRun == false) return;

	rogueData->Update(_dt);
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
		RogueHUD->ItemBoxClass = ItemBoxClass;

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
		MainMenu->QuitButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::QuitGame);

		MainMenu->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create Main Menu")));
		return;
	}

	// TODO: move this
	if (RogueHUD->CardSelectionClass)
	{
		RogueHUD->CardSelection = CreateWidget<URogueCardSelection>(GetPlayerController(), RogueHUD->CardSelectionClass);

		RogueHUD->CardSelection->SetVisibility(ESlateVisibility::Hidden);
		RogueHUD->CardSelection->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("!!! Could not create CardSelection")));
	}
	if (RogueHUD->RelicSelectionClass)
	{
		RogueHUD->RelicSelection = CreateWidget<URogueCardSelection>(GetPlayerController(), RogueHUD->RelicSelectionClass);

		RogueHUD->RelicSelection->CardLeft->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::RelicCardSelectLeft);
		RogueHUD->RelicSelection->CardMiddle->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::RelicCardSelectMiddle);
		RogueHUD->RelicSelection->CardRight->SelectButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::RelicCardSelectRight);

		RogueHUD->RelicSelection->SetVisibility(ESlateVisibility::Hidden);
		RogueHUD->RelicSelection->AddToViewport();
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
			HUD->ItemBoxClass = ItemBoxClass;

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
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString(TEXT("Finished saving!")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("Save failed...")));
	}

}

void ADwarfPlayerState::OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UDwarfSaveGame* save = Cast<UDwarfSaveGame>(LoadedGameData);
	SetupResourceUpgrades();
	ResetDwarfStats();

	if (save == nullptr)
	{
		// Default first time init
		idleCave.GenerateStart();
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString(TEXT("First time launching the game.")));
		// Upgrades
		for (UpgradeType upgrade = (UpgradeType)0; upgrade < UpgradeType::UPGRADE_COUNT; upgrade = (UpgradeType)(1 + upgrade))
		{
			resourceUpgrades[upgrade]->GetCostAndCache();
			resourceUpgrades[upgrade]->UpdateTooltipText();
		}

		HUD->SetDistanceText(currentCave->first->Data.index / 4);

		gameLoaded = true;
		return;
	}

	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UpgradeType::UPGRADE_COUNT; upgrade = (UpgradeType)(1 + upgrade))
	{
		for (unsigned int i = 1; i <= save->upgradeLevels[upgrade]; i++)		// Re-apply upgrade
		{
			resourceUpgrades[upgrade]->upgradeLevel++;
			resourceUpgrades[upgrade]->ApplyUpgrade();
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::FromInt(i));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(resourceUpgrades[upgrade]->upgradeLevel));

		resourceUpgrades[upgrade]->GetCostAndCache();
		resourceUpgrades[upgrade]->UpdateTooltipText();
	}

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

	// Relics
	for (int i = 0; i < 8; i++)
	{
		if (relics[i] == nullptr) continue;
		int count = save->relicCounts[relics[i]->tag];
		if (count == 0) continue;
		
		AddRelic(relics[i], count);
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Relic count: " + FString::FromInt(count));
	}

	idleCave.blocksGenerated = save->blockBroken;
	idleCave.blocksBroken = save->blockBroken;
	while (idleCave.CheckRank());
	idleCave.GenerateStart();
	HUD->SetDistanceText(currentCave->first->Data.index / 4);

	// Get time since last connection
	unsigned long timeSince = difftime(time(nullptr), save->saveTime);
	
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Time since last connection: " + FString::FromInt(timeSince) + "s");
	//Tick(timeSince); // IS THIS EVEN ALLOWED??? TODO: see if this should be the way to catchup or not
	//TimeBoost = true;
	//TimeBoostFactor = fmin(timeSince, 1000.0f);
	//TimeBoostLeft = timeSince / TimeBoostFactor;
	//currentCave->dwarfPawn->TimeFactor = TimeBoostFactor;

	gameLoaded = true;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString(TEXT("Finished loading!")));
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
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString(TEXT("Saving...")));

	FAsyncSaveGameToSlotDelegate SavedDelegate;
	SavedDelegate.BindUObject(this, &ADwarfPlayerState::OnSaveFinished);

	UDwarfSaveGame* save = (UDwarfSaveGame*)UGameplayStatics::CreateSaveGameObject(UDwarfSaveGame::StaticClass());
	
	// Upgrades
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UpgradeType::UPGRADE_COUNT; upgrade = (UpgradeType)(upgrade + 1))
	{
		save->upgradeLevels[upgrade] = resourceUpgrades[upgrade]->upgradeLevel;	// Set level
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

	for (int i = 0; i < 8; i++)
	{
		if (relics[i] == nullptr) continue;
		save->relicCounts[relics[i]->tag] = relics[i]->cumulativeCount;
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
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UpgradeType::UPGRADE_COUNT; upgrade = (UpgradeType)(upgrade + 1))
	{
		save->upgradeLevels[upgrade] = resourceUpgrades[upgrade]->upgradeLevel;	// Set level
	}
	memcpy(save->resources, resources, sizeof(int) * RESOURCE_COUNT);
	memcpy(save->isResourceUnlocked, isResourceUnlocked, sizeof(bool) * RESOURCE_COUNT);

	save->savedStats[0] = savedStats.blocksBroken.value;
	save->savedStats[1] = savedStats.damageDone.value;
	save->savedStats[2] = savedStats.metersWalked.value;
	save->savedStats[3] = savedStats.rebirthCount.value;

	save->level = Level;
	save->exp = Experience;
	
	for (int i = 0; i < 8; i++)
	{
		if (relics[i] == nullptr) continue;
		save->relicCounts[relics[i]->tag] = relics[i]->cumulativeCount;
	}

	save->blockBroken = idleCave.blocksBroken;

	save->saveTime = time(nullptr);

	UGameplayStatics::SaveGameToSlot(save, "SaveSlot", 0);
}

void ADwarfPlayerState::Rebirth()
{
	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i]->upgradeLevel = 0;
		resourceUpgrades[i]->UpdateTooltipText();
		resourceUpgrades[i]->GetCostAndCache();
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
	HUD->SetDistanceText(currentCave->first->Data.index / 4);
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
	TArray<ResourceData> cost = resourceUpgrades[_upgrade]->GetCostAndCache();
	if (PayCost(cost) == false)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Not enough resources"); 
		return;
	}

	// Give upgrade reward based on type
	resourceUpgrades[_upgrade]->LevelUpgrade();
	
	resourceUpgrades[_upgrade]->UpdateTooltipText();
	resourceUpgrades[_upgrade]->GetCostAndCache();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Bought upgrade " + FString::FromInt(_upgrade));
}

FString ADwarfPlayerState::CreateCostText(UpgradeType _upgrade, const TArray<ResourceData>& _cost)
{
	FString string = "Cost (x" + FString::FromInt(resourceUpgrades[_upgrade]->buyMultiplier) + "): \n";
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
		string += resource.Amount.ToStringTrunc();
		if (resourceAvailable == false) string += "</>";
		string += "\n";

	}

	return string;
}

int ADwarfPlayerState::GetMaxUpgradeMult(UpgradeType _upgrade)
{
	TArray<ResourceData> cost;
	cost = resourceUpgrades[_upgrade]->GetCost(resourceUpgrades[_upgrade]->upgradeLevel);
	int maxMult = 1;

	int typeCount = cost.Num();
	for (int i = 1; i < MAX_MULTIPLIER_VALUE; i++)
	{
		TArray<ResourceData> currentCost = resourceUpgrades[_upgrade]->GetCost(resourceUpgrades[_upgrade]->upgradeLevel + i);
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

void ADwarfPlayerState::RelicCardSelectLeft()
{
	RogueHUD->RelicSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData->Resume();
	AddRelic(relicCardSelection[0]);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Left selected")));
	EndRun();
}
void ADwarfPlayerState::RelicCardSelectMiddle()
{
	RogueHUD->RelicSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData->Resume();
	AddRelic(relicCardSelection[1]);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Middle selected")));
	EndRun();
}
void ADwarfPlayerState::RelicCardSelectRight()
{
	RogueHUD->RelicSelection->SetVisibility(ESlateVisibility::Hidden);
	rogueData->Resume();
	AddRelic(relicCardSelection[2]);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Right selected")));
	EndRun();
}

void ADwarfPlayerState::AddRelic(IdleRelic* _relic)
{
	int relicIndex = obtainedRelics.Find(_relic);
	if (relicIndex == INDEX_NONE) // New relic
	{
		_relic->IncreaseCount(1);

		obtainedRelics.Add(_relic);
		_relic->Bind(this, &idleCave);
		
		_relic->widget = HUD->AddRelicWidget(_relic);
	}

	else
	{
		_relic->IncreaseCount(1);
		_relic->widget->descriptionText = _relic->GetDescriptionText(_relic->rank);
		_relic->widget->level = _relic->rank;
	}

	Drill.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_DRILL]->upgradeLevel);
	Boom.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_BOOM]->upgradeLevel);
	Earthquake.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_EARTHQUAKE]->upgradeLevel);
	Laser.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_LASER]->upgradeLevel);
}

void ADwarfPlayerState::AddRelic(IdleRelic* _relic, int _count)
{
	obtainedRelics.Add(_relic);
	_relic->Bind(this, &idleCave);
	// Add to box
	UUpgradeEntryWidget* widget = CreateWidget<UUpgradeEntryWidget, UWrapBox*>(HUD->RelicBox, ItemBoxClass);
	_relic->IncreaseCount(_count);
	widget->level = _relic->rank;
	widget->name = _relic->name;
	if (!IsValid(_relic->icon))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Icon invalid");
		return;
	}
	widget->Icon->SetBrushFromTexture(_relic->icon);
	widget->SetPadding(16);
	widget->LevelText->SetText(FText::FromString(FString::FromInt(widget->level)));
	widget->descriptionText = _relic->GetDescriptionText(_relic->rank);
	HUD->RelicBox->AddChild(widget);

	_relic->widget = widget;

	Drill.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_DRILL]->upgradeLevel);
	Boom.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_BOOM]->upgradeLevel);
	Earthquake.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_EARTHQUAKE]->upgradeLevel);
	Laser.UpdateDisplayTooltip(resourceUpgrades[UpgradeType::UPG_LASER]->upgradeLevel);
}

void ADwarfPlayerState::SetUpgradeMult1()
{
	maxMultiplier = false;
	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i]->SetBuyMultiplier(1);
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
	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i]->SetBuyMultiplier(5);
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
	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i]->SetBuyMultiplier(10);
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
	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i]->SetBuyMultiplier(25);
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
	for (int i = 0; i < UpgradeType::UPGRADE_COUNT; i++)
	{
		resourceUpgrades[i]->SetBuyMultiplier(100);
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
	Level += 1;
	// Do stuff per level (ex: Give tech point, check for level milestone, etc.)
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, "Level up");
	TalentPoints++;
	// Show the tech point
	HUD->TechPointIndicator->SetVisibility(ESlateVisibility::Visible);

	// Compute new Req exp
	RequiredExperience = (Level + 1) * (Level + 1);
}

FString ADwarfPlayerState::GetUpgradeDamageText(UpgradeType _upgrade)
{
	switch (_upgrade)
	{
	case UpgradeType::UPG_STRONG_ARMS: return MinDamage.ToStringTrunc() + "-" + MaxDamage.ToStringTrunc();
	case UpgradeType::UPG_DRILL: return Drill.GetDPS().ToStringTrunc() + "/s";
	case UpgradeType::UPG_BOOM: return Boom.GetDPS().ToStringTrunc() + "/s";
	default: return "Unknown Upgrade type";
	}
}

BigNumber ADwarfPlayerState::GetClickDamage()
{
	BigNumber finalMinDamage = MinDamage * ClickDamageMultiplier;
	MaxDamage = finalMinDamage * DamageWindow;
	BigNumber DamageDelta = MaxDamage - finalMinDamage;
	BigNumber DamageBonus = rand() % ((int)DamageDelta + 1);
	BigNumber finalDamage = finalMinDamage + DamageBonus;

	BigNumber critChanceTemp = CritChance; // Allows for "overcrit"
	while (critChanceTemp > rand() % 100)
	{
		finalDamage *= CritMultiplier;
		critChanceTemp -= 100;
	}

	return finalDamage;
}

void ADwarfPlayerState::Hit(bool _silent = false)
{
	if (inRun) return;

	if(_silent == false) OnHit.Broadcast(this);

	currentCave->dwarfPawn->HitAnimation();

	BigNumber damage = GetClickDamage();
	OnClickDamageCalc.Broadcast(damage);
	Damage(damage,  ClickSource, currentCave);
}

void ADwarfPlayerState::Damage(BigNumber _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageFirst(_damage, _source);
}

void ADwarfPlayerState::DamageColumn(BigNumber _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageFirstColumn(_damage, _source);
}

void ADwarfPlayerState::DamageArea(BigNumber _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageArea(_damage, _source);
}

void ADwarfPlayerState::DamageRow(BigNumber _damage, DamageSource _source, Cave* _cave)
{
	if (!_cave) return;
	if (!_cave->first) return;

	_damage *= _source.BlockDamageMultiplier[_cave->first->Data.type];

	_cave->DamageRow(_damage, _source);
}

void ADwarfPlayerState::DamageIdleCave(BigNumber _damage, DamageSource _source)
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

	HUD->SetDistanceText(currentCave->first->Data.index / 4);
}

void ADwarfPlayerState::FocusIdle()
{
	CameraActor->SetState(ADwarfCameraActor::IDLE);
	MainMenu->SetVisibility(ESlateVisibility::Hidden);
	HUD->SetVisibility(ESlateVisibility::Visible);

	CameraActor->pawn = currentCave->dwarfPawn;
	HUD->SetDistanceText(currentCave->first->Data.index / 4);
}

void ADwarfPlayerState::FocusRogue()
{
	CameraActor->SetState(ADwarfCameraActor::IDLE);
	MainMenu->SetVisibility(ESlateVisibility::Hidden);
	RogueHUD->SetVisibility(ESlateVisibility::Visible);

	CameraActor->pawn = currentCave->dwarfPawn;
	RogueHUD->SetDistanceText(currentCave->first->Data.index / 4);
}

void ADwarfPlayerState::FocusMenu()
{
	CameraActor->SetState(ADwarfCameraActor::MENU);
	HUD->SetVisibility(ESlateVisibility::Hidden);
	MainMenu->SetVisibility(ESlateVisibility::Visible);

	CameraActor->pawn = currentCave->dwarfPawn;
}

void ADwarfPlayerState::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void ADwarfPlayerState::StartRun()
{
	inRun = true;
	rogueData = NewObject<URoguePlayerData>();

	// Build the item pool
	BuildItemPool();

	// Hide idle cave
	idleCave.SetCaveVisible(false);

	// Create new rogue cave
	currentCave = new RogueCave(); // TODO: Create once and reset between runs
	rogueData->currentCave = currentCave;

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	currentCave->dwarfPawn = GetWorld()->SpawnActor<ADwarfPawn>(DwarfPawnClass, FVector(), FRotator(), param);
	if (!currentCave->dwarfPawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Cannot instanciate pawn");
		return;
	}

	currentCave->BlockBreakDelegate.BindUObject(rogueData, &URoguePlayerData::BlockReward);
	currentCave->DamageTextClass = DamageTextClass;
	currentCave->GenerateStart();
	currentCave->dwarfPawn->PositionToCave(currentCave);

	rogueData->RogueHUD = RogueHUD;
	RogueHUD->CardSelection->SetRogueData(rogueData);
	RogueHUD->SetLevel(rogueData->level, (float)rogueData->experience / (float)rogueData->experienceRequired);

	FocusRogue();
}

void ADwarfPlayerState::BuildItemPool()
{
	unlockedItemPool.Empty();
	unlockedItemPool.Add(NewObject<UDamageRogueItem>(this));
	unlockedItemPool.Add(NewObject<UMultihitRogueItem>(this));
	unlockedItemPool.Add(NewObject<UCooldownRogueItem>(this));
	unlockedItemPool.Add(NewObject<UDrillRogueItem>(this));
	unlockedItemPool.Add(NewObject<UTNTRogueItem>(this));
	unlockedItemPool.Add(NewObject<UEarthquakeRogueItem>(this));
	unlockedItemPool.Add(NewObject<ULaserRogueItem>(this));
	unlockedItemPool.Add(NewObject<UResistanceRogueItem>(this));
	unlockedItemPool.Add(NewObject<URegenRogueItem>(this));
	for (int i = 0; i < 9; i++)
	{
		unlockedItemPool[i]->associatedRelic = relics[i];
	}

	rogueData->currentItemPool = unlockedItemPool;
}

void ADwarfPlayerState::RunRewards()
{
	TArray<URogueItem*> tempItems = rogueData->items;
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

	RogueHUD->RelicSelection->SetVisibility(ESlateVisibility::Visible);
	rogueData->Pause();

	UItemCard* cards[3];
	cards[0] = RogueHUD->RelicSelection->CardLeft;	RogueHUD->RelicSelection->CardLeft->SetVisibility(ESlateVisibility::Hidden);
	cards[1] = RogueHUD->RelicSelection->CardMiddle;	RogueHUD->RelicSelection->CardMiddle->SetVisibility(ESlateVisibility::Hidden);
	cards[2] = RogueHUD->RelicSelection->CardRight;	RogueHUD->RelicSelection->CardRight->SetVisibility(ESlateVisibility::Hidden);

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

	rogueData->currentItemPool.Empty();
	rogueData->items.Empty();
	for (int i = 0; i < rogueData->itemWidgets.Num(); i++)
	{
		rogueData->itemWidgets[i]->RemoveFromParent();
	}
	rogueData->itemWidgets.Empty();

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
			switch (_damager.damageType)
			{
			case FIRST:
			{
				Damage(_damager.GetDamage(), _damager.source, &idleCave);
				break;
			}
			case COLUMN:
			{
				DamageColumn(_damager.GetDamage(), _damager.source, &idleCave);
				break;
			}
			case AREA:
			{
				DamageArea(_damager.GetDamage(), _damager.source, &idleCave);
				break;
			}
			case LINE:
			{
				DamageRow(_damager.GetDamage(), _damager.source, &idleCave);
				break;
			}
			default:
			{
				Damage(_damager.GetDamage(), _damager.source, &idleCave);
				break;
			}
			}
			
		}
	}
}

BigNumber AutomaticDamager::GetDamage()
{
	BigNumber damage = Damage;
	OnDamageCalc.Broadcast(damage);
	return damage;
}

float AutomaticDamager::GetCooldown()
{
	float cooldown = Downtime;
	OnCooldownCalc.Broadcast(cooldown);
	return cooldown;
}

bool AutomaticDamager::IsHitting()
{
	bool isHitting = Clock <= 0;

	if (isHitting)
	{
		Clock += GetCooldown();
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
	Display->DamageText = GetDamage().ToStringTrunc();
	Display->hitCooldown = GetCooldown();
	Display->level = _level;
}
