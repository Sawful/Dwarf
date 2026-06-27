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

	DwarfPawn = (ADwarfPawn*)GetPawn();
	PState = (ADwarfPlayerState*)PlayerState;
	PState->pawn = DwarfPawn;
	GState = (ADwarfGameState*)GetWorld()->GetGameState();

	SetShowMouseCursor(true);
}

void ADwarfPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LClick", IE_Pressed, this, &ADwarfPlayerController::Hit);
	InputComponent->BindAction("RClick", IE_Pressed, this, &ADwarfPlayerController::_AutoclickOn);
	InputComponent->BindAction("RClick", IE_Released, this, &ADwarfPlayerController::_AutoclickOff);
}

void ADwarfPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADwarfPlayerController::Tick(float DeltaTime)
{
	if (_autoclick) Hit();
}

void ADwarfPlayerController::Hit()
{
	PState->Hit();
}

void ADwarfPlayerController::_AutoclickOn()
{
	_autoclick = true;
}

void ADwarfPlayerController::_AutoclickOff()
{
	_autoclick = false;
}
