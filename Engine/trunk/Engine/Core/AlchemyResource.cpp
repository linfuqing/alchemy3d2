#include "AlchemyResource.h"
#include "AlchemyNull.h"

using namespace alchemy;

volatile void* CResource::sm_pPointer = ALCHEMY_NULL;

CResource::CResource(void) : m_pPointer( const_cast<void*>(sm_pPointer) ), m_uReferenceCount(0)
{
	sm_pPointer = ALCHEMY_NULL;
}

CResource::~CResource(void)
{

}

void* CResource::operator new(size_t uSize)
{
	while(sm_pPointer);

	sm_pPointer = ALCHEMY_NEW(uSize);

	return const_cast<void*>(sm_pPointer);
}