#include "DwarfPlayerController.h"
#include "DwarfPawn.h"
#include "DwarfPlayerState.h"
#include "Block.h"
#include "Cave.h"

ADwarfPlayerController::ADwarfPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ADwarfPlayerController::BeginPlay()
{
	Super::BeginPlay();
	

	SetShowMouseCursor(true);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("Controller Begin"));
}

void ADwarfPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(IsValid(PState)));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(IsValid(GState)));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(IsValid(DwarfPawn)));

	InputComponent->BindAction("LClick", IE_Pressed, this, &ADwarfPlayerController::Hit);
	InputComponent->BindAction("RClick", IE_Pressed, this, &ADwarfPlayerController::_AutoclickOn);
	InputComponent->BindAction("RClick", IE_Released, this, &ADwarfPlayerController::_AutoclickOff);
}

void ADwarfPlayerController::TryStartGame()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Try start game");
	DwarfPawn = GetPawn<ADwarfPawn>();
	if (!DwarfPawn)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Pawn");
		return;
	}
	PState = (ADwarfPlayerState*)PlayerState;
	if (!PState) 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "PState");
		return;
	}
	PState->idlePawn = DwarfPawn;

	GState = (ADwarfGameState*)GetWorld()->GetGameState();
	if (!GState) 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "GState");
		return;
	}
	validStart = true;
	PState->StartGame();
}

void ADwarfPlayerController::Tick(float DeltaTime)
{
	if (!validStart)
	{
		TryStartGame();
		return;
	}

	Super::Tick(DeltaTime);

	if (_autoclick) Hit();
}

void ADwarfPlayerController::Hit()
{
	PState->Hit(false);
}

void ADwarfPlayerController::_AutoclickOn()
{
	_autoclick = true;
}

void ADwarfPlayerController::_AutoclickOff()
{
	_autoclick = false;
}
