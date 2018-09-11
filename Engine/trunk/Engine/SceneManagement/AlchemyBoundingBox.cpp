#include "AlchemyBoundingBox.h"

using namespace alchemy;

CBoundingBox::CBoundingBox(const BOUNDINGBOX& BoundingBox)
{
	Copy(*this, BoundingBox);
}

CBoundingBox::~CBoundingBox(void)
{
}
