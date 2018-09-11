#include "AlchemyFlashVolumeLightShow.h"

using namespace alchemy;

CFlashVolumeLightShow::CFlashVolumeLightShow()
{

}

CFlashVolumeLightShow::~CFlashVolumeLightShow()
{

}

#ifdef AS3_H
AS3TYPE CFlashVolumeLightShow::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType",
		&m_ConstantData[DENSITY_INDEX], 
		&m_ConstantData[SAMPLES_NUM_INDEX],
		&m_ConstantData[EXPOSURE_INDEX],
		&m_ConstantData[DECAY_INDEX],
		&m_fScatterWeight);
}
#endif