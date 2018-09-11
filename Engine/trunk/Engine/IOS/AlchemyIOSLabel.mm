//
//  AlchemyLabel.mm
//  IOS
//
//  Created by lin fuqing on 12-5-16.
//  Copyright 2012 AstepGame. All rights reserved.
//

#import "AlchemyIOSLabel.h"

using namespace alchemy;

CIOSLabel::CIOSLabel()
{
}

CIOSLabel::~CIOSLabel()
{
}

UIView* CIOSLabel::_CreateView(const CGRect& Rect)
{
	UILabel* pLabel = [[UILabel alloc] initWithFrame:Rect];
	pLabel.backgroundColor = 0;
	pLabel.opaque = NO;
	
	return pLabel;
}