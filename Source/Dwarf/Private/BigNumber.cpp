#include "BigNumber.h"

BigNumber BigNumber::operator+(BigNumber _other)
{
	BigNumber result;
	if (exponent == _other.exponent)
	{
		result.mantissa = mantissa + _other.mantissa;
		result.exponent = exponent;
	}
	else if (exponent < _other.exponent)
	{
		result.mantissa = mantissa * powf(0.1f, _other.exponent - exponent) + _other.mantissa;
		result.exponent = _other.exponent;
	}
	else
	{
		result.mantissa = mantissa + _other.mantissa * powf(0.1f, exponent - _other.exponent);
		result.exponent = exponent;
	}

	while (abs(result.mantissa) >= 10)
	{
		result.mantissa /= 10;
		result.exponent++;
	}
	while (abs(result.mantissa) < 1)
	{
		result.mantissa *= 10;
		result.exponent--;
	}
	return result;
}

BigNumber BigNumber::operator-(BigNumber _right)
{
	BigNumber result;
	if (exponent == _right.exponent)
	{
		result.mantissa = mantissa - _right.mantissa;
		result.exponent = exponent;
	}
	else if (exponent < _right.exponent)
	{
		result.mantissa = mantissa * powf(0.1f, _right.exponent - exponent) - _right.mantissa;
		result.exponent = _right.exponent;
	}
	else
	{
		result.mantissa = mantissa - _right.mantissa * powf(0.1f, exponent - _right.exponent);
		result.exponent = exponent;
	}

	while (abs(result.mantissa) >= 10)
	{
		result.mantissa /= 10;
		result.exponent++;
	}
	while (abs(result.mantissa) < 1)
	{
		result.mantissa *= 10;
		result.exponent--;
	}
	return result;
}

BigNumber BigNumber::operator*(BigNumber _other)
{
	BigNumber result;
	result.mantissa = mantissa * _other.mantissa;
	result.exponent = exponent + _other.exponent;

	while (abs(result.mantissa) >= 10)
	{
		result.mantissa /= 10;
		result.exponent++;
	}
	while (abs(result.mantissa) < 1)
	{
		result.mantissa *= 10;
		result.exponent--;
	}
	return result;
}

BigNumber BigNumber::operator/(BigNumber _right)
{
	BigNumber result;
	result.mantissa = mantissa / _right.mantissa;
	result.exponent = exponent - _right.exponent;

	while (abs(result.mantissa) >= 10)
	{
		result.mantissa /= 10;
		result.exponent++;
	}
	while (abs(result.mantissa) < 1)
	{
		result.mantissa *= 10;
		result.exponent--;
	}
	return result;
}

BigNumber& BigNumber::operator+=(BigNumber _other)
{
	if (exponent == _other.exponent)
	{
		mantissa = mantissa + _other.mantissa;
		exponent = exponent;
	}
	else if (exponent < _other.exponent)
	{
		mantissa = mantissa * powf(0.1f, _other.exponent - exponent) + _other.mantissa;
		exponent = _other.exponent;
	}
	else
	{
		mantissa = mantissa + _other.mantissa * powf(0.1f, exponent - _other.exponent);
		exponent = exponent;
	}

	while (abs(mantissa) >= 10)
	{
		mantissa /= 10;
		exponent++;
	}
	while (abs(mantissa) < 1)
	{
		mantissa *= 10;
		exponent--;
	}
	return *this;
}

BigNumber& BigNumber::operator-=(BigNumber _right)
{
	if (exponent == _right.exponent)
	{
		mantissa = mantissa - _right.mantissa;
		exponent = exponent;
	}
	else if (exponent < _right.exponent)
	{
		mantissa = mantissa * powf(0.1f, _right.exponent - exponent) - _right.mantissa;
		exponent = _right.exponent;
	}
	else
	{
		mantissa = mantissa - _right.mantissa * powf(0.1f, exponent - _right.exponent);
		exponent = exponent;
	}

	while (abs(mantissa) >= 10)
	{
		mantissa /= 10;
		exponent++;
	}
	while (abs(mantissa) < 1)
	{
		mantissa *= 10;
		exponent--;
	}
	return *this;
}

BigNumber& BigNumber::operator*=(BigNumber _other)
{
	mantissa = mantissa * _other.mantissa;
	exponent = exponent + _other.exponent;

	while (abs(mantissa) >= 10)
	{
		mantissa /= 10;
		exponent++;
	}
	while (abs(mantissa) < 1)
	{
		mantissa *= 10;
		exponent--;
	}
	return *this;
}

BigNumber& BigNumber::operator/=(BigNumber _right)
{
	mantissa = mantissa / _right.mantissa;
	exponent = exponent - _right.exponent;

	while (abs(mantissa) >= 10)
	{
		mantissa /= 10;
		exponent++;
	}
	while (abs(mantissa) < 1)
	{
		mantissa *= 10;
		exponent--;
	}
	return *this;
}

bool BigNumber::operator<(BigNumber _right)
{
	if (exponent < _right.exponent) return true;
	if (exponent > _right.exponent) return false;
	return mantissa < _right.mantissa;
}

bool BigNumber::operator>(BigNumber _right)
{
	if (exponent > _right.exponent) return true;
	if (exponent < _right.exponent) return false;
	return mantissa > _right.mantissa;
}

bool BigNumber::operator<=(BigNumber _right)
{
	if (exponent < _right.exponent) return true;
	if (exponent > _right.exponent) return false;
	return mantissa <= _right.mantissa;
}

bool BigNumber::operator>=(BigNumber _right)
{
	if (exponent > _right.exponent) return true;
	if (exponent < _right.exponent) return false;
	return mantissa >= _right.mantissa;
}

bool BigNumber::operator==(BigNumber _right)
{
	return false;
}

BigNumber::BigNumber()
{
	mantissa = 0;
	exponent = 0;
}

BigNumber::BigNumber(float _value)
{
	exponent = 0;
	if (_value == 0)
	{
		mantissa = _value;
		return;
	}

	while (abs(_value) >= 10)
	{
		_value /= 10;
		exponent++;
	}
	while (abs(_value) < 1)
	{
		_value *= 10;
		exponent++;
	}
	mantissa = _value;
}

BigNumber::BigNumber(double _value)
{
	exponent = 0;
	if (_value == 0)
	{
		mantissa = _value;
		return;
	}

	while (abs(_value) >= 10)
	{
		_value /= 10;
		exponent++;
	}
	while (abs(_value) < 1)
	{
		_value *= 10;
		exponent++;
	}
	mantissa = _value;
}

BigNumber::BigNumber(int _value)
{
	float pointValue = _value;
	exponent = 0;
	while (abs(pointValue) >= 10)
	{
		pointValue /= 10;
		exponent++;
	}
	mantissa = pointValue;
}

BigNumber::BigNumber(unsigned int _value)
{
	float pointValue = _value;
	exponent = 0;
	while (pointValue >= 10)
	{
		pointValue /= 10;
		exponent++;
	}
	mantissa = pointValue;
}

BigNumber::BigNumber(uint64 _value)
{
	double pointValue = _value;
	exponent = 0;
	while (pointValue >= 10)
	{
		pointValue /= 10;
		exponent++;
	}
	mantissa = pointValue;
}

BigNumber::~BigNumber()
{
}


FString BigNumber::ToString()
{
	if (exponent >= 10 || exponent <= -8) return FString::Printf(TEXT("%.4f"), mantissa) + "E" + FString::Printf(TEXT("%+d"), exponent);
	return FString::Printf(TEXT("%lld"), int64(mantissa * powf(10, exponent)));
}