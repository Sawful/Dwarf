// Fill out your copyright notice in the Description page of Project Settings.


#include "MathTools.h"

//BigNumber MagicFunction(const unsigned int _level, const uint64 _baseValue, const float _s1, const float _s2)
//{
//	return uint64(_baseValue * powf((_level + _s2) / _s2, _s2 * _s1));
//}

BigNumber MagicFunction(const unsigned int _level, const float _baseValue, const float _s1, const float _s2)
{
	return _baseValue * powf((_level + _s2) / _s2, _s2 * _s1);
}
