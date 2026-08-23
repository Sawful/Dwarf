// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCardSelection.h"

void URogueCardSelection::SetRogueData(URoguePlayerData* _rogueData)
{
	rogueData = _rogueData;
	CardLeft->rogueData = rogueData;
	CardMiddle->rogueData = rogueData;
	CardRight->rogueData = rogueData;
}
