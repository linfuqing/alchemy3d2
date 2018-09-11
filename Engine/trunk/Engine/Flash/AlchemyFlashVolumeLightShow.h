#pragma once

#include "../Graphics/AlchemyVolumeLightShow.h"

#include "AlchemyFlashObjectInterface.h"

namespace alchemy
{
	class CFlashVolumeLightShow : 
		public CVolumeLightShow, public CFlashObjectInterface
	{
	public:
		CFlashVolumeLightShow();
		virtual ~CFlashVolumeLightShow();

#ifdef AS3_H
		virtual AS3TYPE Init() ;
#endif
	};
}