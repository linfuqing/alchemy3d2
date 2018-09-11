#include "AlchemyRGBAColor.h"

using namespace alchemy;

CRGBAColor::CRGBAColor(void)
{
}

CRGBAColor::CRGBAColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

CRGBAColor::~CRGBAColor(void)
{
}
