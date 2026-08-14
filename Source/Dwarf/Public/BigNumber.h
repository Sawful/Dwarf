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
	explicit operator float() { return mantissa * powf(10, exponent); };
	explicit operator double() { return mantissa * powf(10, exponent); };
	explicit operator int() { return mantissa * powf(10, exponent); };
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
	FString ToStringTrunc();
};
