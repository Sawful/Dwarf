#pragma once

#include "CoreMinimal.h"

class DWARF_API BigNumber
{
public:
	BigNumber operator+(BigNumber _other);
	BigNumber operator-(BigNumber _right);
	BigNumber operator*(BigNumber _other);
	BigNumber operator/(BigNumber _right);
	BigNumber& operator+=(BigNumber _other);
	BigNumber& operator-=(BigNumber _right);
	BigNumber& operator*=(BigNumber _other);
	BigNumber& operator/=(BigNumber _right);
	bool operator<(BigNumber _right);
	bool operator>(BigNumber _right);
	bool operator<=(BigNumber _right);
	bool operator>=(BigNumber _right);
	bool operator==(BigNumber _right);

	BigNumber();
	BigNumber(float _value);
	BigNumber(double _value);
	BigNumber(int _value);
	BigNumber(unsigned int _value);
	BigNumber(uint64 _value);
	~BigNumber();

public:
	float mantissa;
	int exponent;

	FString ToString();
};
