#pragma once
//
//  AlchemyLabel.h
//  IOS
//
//  Created by lin fuqing on 12-5-16.
//  Copyright 2012 AstepGame. All rights reserved.
//

#include "AlchemyIOSWindow.h"

namespace alchemy
{
	class CIOSLabel :
	public CIOSWindow
	{
	public:
		CIOSLabel();
		~CIOSLabel();
		
	protected:
		UIView* _CreateView(const CGRect& Rect);
	};
}