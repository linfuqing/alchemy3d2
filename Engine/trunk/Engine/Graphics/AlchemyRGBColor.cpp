#include "AlchemyRGBColor.h"

using namespace alchemy;

CRGBColor::CRGBColor(void)
{
}

CRGBColor::CRGBColor(FLOAT r, FLOAT g, FLOAT b)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = 1.0f;
}

CRGBColor::~CRGBColor(void)
{
}
