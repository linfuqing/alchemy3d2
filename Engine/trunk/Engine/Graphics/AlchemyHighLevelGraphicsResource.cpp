#include "AlchemyHighLevelGraphicsResource.h"
#include "AlchemyGraphicsResourcePool.h"

using namespace alchemy;

CHighLevelGraphicsResource::CHighLevelGraphicsResource()
{
}

CHighLevelGraphicsResource::~CHighLevelGraphicsResource(void)
{
}

bool CHighLevelGraphicsResource::_Create()
{
	return true;
}

CHighLevelGraphicsResource* CHighLevelGraphicsResource::_Clone()
{
	return static_cast<CHighLevelGraphicsResource*>( GetGraphicsResourcePool().CreateResource( GetType() ) );
}