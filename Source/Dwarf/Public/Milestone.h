#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FOnMilestoneTier, int);
template <typename T>
struct DWARF_API Milestone
{
	T value; // Tracked value
	int currentTier = 0;
	int maximumTier;
	TArray<T> tiers;
	FOnMilestoneTier tierUpDelegate;

	Milestone(T _value) : value(_value) {};

	inline void CheckTier()
	{
		while (currentTier < maximumTier && value > tiers[currentTier])
		{
			// Call milestone function
			tierUpDelegate.Execute(currentTier);
			currentTier++;
		}
	}
};