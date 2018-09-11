/*
 *  AlchemyVector2.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-30.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyVector2.h"

using namespace alchemy;

CVector2::CVector2()
{
}

CVector2::CVector2(FLOAT x, FLOAT y) :
CFloat2(x, y)
{
}

CVector2::~CVector2()
{
}

void CVector2::Normalize()
{
	*this *= 1.0f / GetLength();
}