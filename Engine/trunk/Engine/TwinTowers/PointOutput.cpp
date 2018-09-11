#include "PointOutput.h"

#include "../Platform/AlchemyPlatformManager.h"

using namespace alchemy;

CPointOutput::CPointMove::CPointMove(CPointOutput& Parent) : m_Parent(Parent) 
{

}

CPointOutput::CPointMove::~CPointMove() 
{

}

void CPointOutput::CPointMove::Handler(const IEventDispatcher::CEvent& Event)
{
	const IWindow::CPointEvent& PointEvent = static_cast<const IWindow::CPointEvent&>(Event);
	const FLOAT2* pPoints = PointEvent.GetPoints();
	UINT uPointCount = PointEvent.GetPointCount();
	if(uPointCount > 1)
	{
		if( ALCHEMY_TEST_BIT(m_Parent.m_uFlag, CPointOutput::SCALE) )
		{
			UINT uType;

			if( ALCHEMY_GET_EVENT_TYPE(uType, SCALE, CPointOutput) )
			{
				FLOAT x = m_Parent.m_Destination.x - m_Parent.m_Source.x, y = m_Parent.m_Destination.y - m_Parent.m_Source.y, fLength = sqrtf(x * x + y * y);
				const FLOAT2& Source = pPoints[0], &Destination = pPoints[1];

				m_Parent.m_Source.x = Source.x;
				m_Parent.m_Source.y = Source.y;

				m_Parent.m_Destination.x = Destination.x;
				m_Parent.m_Destination.y = Destination.y;

				x = Destination.x - Source.x;
				y = Destination.y - Source.y;

				CPointOutput::CPointEvent PointEvent(sqrtf(x * x + y * y) - fLength, uType);
				m_Parent.DispatchEvent(PointEvent);
			}
		}
	}
	else if(uPointCount)
	{
		const FLOAT2& Point = pPoints[0];
		bool bIsTransverse = ALCHEMY_TEST_BIT(m_Parent.m_uFlag, CPointOutput::TRANSVERSE), bIsVertical = ALCHEMY_TEST_BIT(m_Parent.m_uFlag, CPointOutput::VERTICAL);

		if(bIsTransverse && bIsVertical)
		{
			UINT uType;

			FLOAT x = m_Parent.m_Source.x - Point.x, y = Point.y - m_Parent.m_Source.y;
			if( ALCHEMY_ABSOLUTE(x) > ALCHEMY_ABSOLUTE(y) )
			{
				if( ALCHEMY_GET_EVENT_TYPE(uType, TRANSVERSE, CPointOutput) )
				{
					CPointOutput::CPointEvent PointEvent(Point.x - m_Parent.m_Source.x, uType);
					m_Parent.DispatchEvent(PointEvent);
				}
			}
			else
			{
				if( ALCHEMY_GET_EVENT_TYPE(uType, VERTICAL, CPointOutput) )
				{
					CPointOutput::CPointEvent PointEvent(Point.y - m_Parent.m_Source.y, uType);
					m_Parent.DispatchEvent(PointEvent);
				}
			}
		}
		else if(bIsTransverse)
		{
			UINT uType;
			if( ALCHEMY_GET_EVENT_TYPE(uType, TRANSVERSE, CPointOutput) )
			{
				CPointOutput::CPointEvent PointEvent(Point.x - m_Parent.m_Source.x, uType);
				m_Parent.DispatchEvent(PointEvent);
			}
		}
		else if(bIsVertical)
		{
			UINT uType;
			if( ALCHEMY_GET_EVENT_TYPE(uType, VERTICAL, CPointOutput) )
			{
				CPointOutput::CPointEvent PointEvent(Point.y - m_Parent.m_Source.y, uType);
				m_Parent.DispatchEvent(PointEvent);
			}
		}

		m_Parent.m_Source.x = Point.x;
		m_Parent.m_Source.y = Point.y;
	}

	m_Parent.m_bIsMove = true;
}

CPointOutput::CPointOn::CPointOn(CPointOutput& Parent) : m_Parent(Parent) 
{

}

CPointOutput::CPointOn::~CPointOn() 
{
}


void CPointOutput::CPointOn::Handler(const IEventDispatcher::CEvent& Event)
{
	const IWindow::CPointEvent& PointEvent = static_cast<const IWindow::CPointEvent&>(Event);
	const FLOAT2* pPoints = PointEvent.GetPoints();
	UINT uPointCount = PointEvent.GetPointCount();
	if(uPointCount)
	{
		if(m_Parent.m_uPointCount)
		{
			const FLOAT2& Point = pPoints[0];
			m_Parent.m_Destination.x = Point.x;
			m_Parent.m_Destination.y = Point.y;
		}
		else
		{
			const FLOAT2& Point = pPoints[0];
			m_Parent.m_Source.x = Point.x;
			m_Parent.m_Source.y = Point.y;
		}

		++ m_Parent.m_uPointCount;

		m_Parent.m_bIsMove = false;
	}
}


CPointOutput::CPointOut::CPointOut(CPointOutput& Parent) : m_Parent(Parent) 
{

}
CPointOutput::CPointOut::~CPointOut() 
{

}

void CPointOutput::CPointOut::Handler(const IEventDispatcher::CEvent& Event)
{
	-- m_Parent.m_uPointCount;

	if(m_Parent.m_bIsMove)
	{
		if(!m_Parent.m_uPointCount)
			m_Parent.m_bIsMove = false;
	}
	else
	{
		if( ALCHEMY_TEST_BIT(m_Parent.m_uFlag, CPointOutput::CLICK) )
		{
			UINT uType;
			if( ALCHEMY_GET_EVENT_TYPE(uType, CLICK, CPointOutput) )
			{
				IEventDispatcher::CEvent Event(uType);
				m_Parent.DispatchEvent(Event);
			}
		}
	}
}

CPointOutput::CPointEvent::CPointEvent(FLOAT fValue, UINT uType, bool bIsBubbles) :
m_fValue(fValue), 
CEvent(uType, bIsBubbles)
{

}

CPointOutput::CPointEvent::~CPointEvent()
{
}

CPointOutput::CPointOutput() : 
m_uPointCount(0), 
m_bIsMove(false), 
m_bIsRun(false), 
m_Move(*this), 
m_On(*this), 
m_Out(*this), 
m_uFlag(0)
{

}

CPointOutput::~CPointOutput()
{

}

bool CPointOutput::Run()
{
	if(m_bIsRun)
		return false;

	IWindow* pWindow = ALCHEMY_PLATFORM_MANAGER.GetWindow(1);
	if(pWindow)
	{
		UINT uType;

		if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_MOVE, IWindow) )
			pWindow->AddEventListener(uType, m_Move);

		if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_ON, IWindow) )
			pWindow->AddEventListener(uType, m_On);

		if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_OUT, IWindow) )
			pWindow->AddEventListener(uType, m_Out);

		m_bIsRun = true;

		return true;
	}

	return false;
}