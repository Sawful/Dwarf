// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfGameModeBase.h"
#include "DwarfPawn.h"

ADwarfGameModeBase::ADwarfGameModeBase() {
	DefaultPawnClass = ADwarfPawn::StaticClass();
}