// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfPlayerState.h"
#include "DwarfPawn.h"
#include "Cave.h"
#include "DwarfUserWidget.h"
#include "Components/TextRenderComponent.h"
#include "DwarfSaveGame.h"

void ADwarfPlayerState::BeginPlay()
{
	if (HUDClass)
	{
		HUD = CreateWidget<UDwarfUserWidget>(GetPlayerController(), HUDClass);
		HUD->UpgradeButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::UpgradeStrongArms);
		HUD->SaveButton->OnClicked.AddDynamic(this, &ADwarfPlayerState::SaveCurrentState);
		HUD->Populate();
		HUD->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not create HUD")));
	}

	cave = new Cave();
	cave->player = this;
	cave->GenerateStart();

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

void ADwarfPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SaveCurrentState();
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
		for (int i = 0; i < UPGRADE_COUNT; i++) resourceUpgrades.upgradeLevels[i] = 0;
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString(TEXT("First time launching the game.")));
		return;
	}

	// Apply upgrades
	memcpy(resourceUpgrades.upgradeLevels, save->upgradeLevels, sizeof(int) * UPGRADE_COUNT);
	memcpy(resources, save->resources, sizeof(int) * RESOURCE_COUNT);
	HUD->UpdateResources(resources);
	for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT;)
	{
		for (int i = 0; i < resourceUpgrades.upgradeLevels[upgrade]; i++)
		{
			IncreaseResourceUpgrade(upgrade);
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(resourceUpgrades.upgradeLevels[upgrade]));
		upgrade = (UpgradeType)(1 + upgrade);
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString(TEXT("Finished loading!")));
}

void ADwarfPlayerState::SaveCurrentState()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString(TEXT("Saving...")));

	// Set up the (optional) delegate.
	FAsyncSaveGameToSlotDelegate SavedDelegate;
	// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
	SavedDelegate.BindUObject(this, &ADwarfPlayerState::OnSaveFinished);

	UDwarfSaveGame* save = (UDwarfSaveGame*)UGameplayStatics::CreateSaveGameObject(UDwarfSaveGame::StaticClass());
	// Set all values
	memcpy(save->upgradeLevels, resourceUpgrades.upgradeLevels, sizeof(int) * UPGRADE_COUNT);
	memcpy(save->resources, resources, sizeof(int) * RESOURCE_COUNT);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(save->upgradeLevels[0]));
	/*for (UpgradeType upgrade = (UpgradeType)0; upgrade < UPGRADE_COUNT;)
	{
		save->resourceUpgradeLevels[upgrade] = resourceUpgrades.upgradeLevels[upgrade];
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::FromInt(save->resourceUpgradeLevels[upgrade]));
		upgrade = (UpgradeType)(1 + upgrade);
	}*/

	UGameplayStatics::AsyncSaveGameToSlot(save, "SaveSlot", 0, SavedDelegate);
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
	TArray<ResourceData> cost = resourceUpgrades.GetCost(_upgrade);
	if (PayCost(cost) == false) return;

	// Give upgrade reward based on type
	resourceUpgrades.upgradeLevels[_upgrade]++;
	IncreaseResourceUpgrade(_upgrade);
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
	}
}

void ADwarfPlayerState::UpgradeStrongArms()
{
	BuyResourceUpgrade(STRONG_ARMS);
}

int ADwarfPlayerState::GetDamage()
{
	int DamageDelta = MaxDamage - MinDamage;
	int DamageBonus = rand() % (DamageDelta + 1);
	return MinDamage + DamageBonus;
}

void ADwarfPlayerState::Hit()
{
	BlockData currentBlockData = cave->first->Data;

	int damage = GetDamage();
	CreateDamageText(damage);
	if (cave->DamageFirst(damage))
	{
		IncreaseBlocks();

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

void ADwarfPlayerState::IncreaseBlocks()
{
	savedStats.blocksBroken++;

	// Check milestones:
	if (savedStats.blocksBroken == 15)
	{
		MinDamage += 5;
		MaxDamage += 5;
	}
}

void ADwarfPlayerState::IncreaseWalk()
{
	savedStats.metersWalked++;

	// Check milestones:
	if (savedStats.metersWalked == 10)
	{
		movementSpeed += 5;
	}
}

void ADwarfPlayerState::CreateDamageText(int _damage)
{
	if (!DamageTextClass) return;

	FVector position = cave->first->GetActorLocation();
	position.X += -55;
	position.Y += ((float)((rand() % 100) - 50));
	position.Z += ((float)((rand() % 100) - 50));

	FRotator rotator;
	rotator.Yaw = 180;
	ATextRenderActor* damageText = GetWorld()->SpawnActor<ATextRenderActor>(DamageTextClass, position, rotator, FActorSpawnParameters());

	damageText->GetTextRender()->SetText(FText::FromString(FString::FromInt(_damage)));
}

TArray<ResourceData> ResourceUpgrades::GetCost(UpgradeType _upgrade)
{
	TArray<ResourceData> Cost;
	if (_upgrade >= UPGRADE_COUNT)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Could not find upgrade.")));
		return Cost;
	}
	
	int level = upgradeLevels[_upgrade];
	switch (_upgrade)
	{
	case STRONG_ARMS:
	{
		Cost.Add({DIRT, 1 + level});
		Cost.Add({STONE, 1 + 2 * level});
		break;
	}
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	//...
	}

	return Cost;
}
