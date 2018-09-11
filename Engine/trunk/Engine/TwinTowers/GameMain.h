#pragma once

#include "Input.h"
#include "PointOutput.h"

#include "Tower.h"

namespace alchemy
{
	class CGameLoop;
	class CGameSceneCreator;
	class CEngineCreator;
	class CGameCreator;
	class CSceneManager;
	//class CTower;
	class CGameMain
	{
		friend class CGameLoop;
		friend class CGameSceneCreator;
		friend class CEngineCreator;
		friend class CGameCreator;

		class CGameInputHandler :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CGameInputHandler(CGameMain& Parent);
			~CGameInputHandler();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CGameMain& m_Parent;
		};

		class CGameTransversePointOutputHandler :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CGameTransversePointOutputHandler(CGameMain& Parent);
			~CGameTransversePointOutputHandler();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CGameMain& m_Parent;
		};

		class CGameVerticalPointOutputHandler :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CGameVerticalPointOutputHandler(CGameMain& Parent);
			~CGameVerticalPointOutputHandler();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CGameMain& m_Parent;
		};

		class CGameScalePointOutputHandler :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CGameScalePointOutputHandler(CGameMain& Parent);
			~CGameScalePointOutputHandler();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CGameMain& m_Parent;
		};

		class CGameClickPointOutputHandler :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CGameClickPointOutputHandler(CGameMain& Parent);
			~CGameClickPointOutputHandler();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CGameMain& m_Parent;
		};
	public:
		/*typedef enum
		{
			ECONOMY,
			MILITARY
		}FIELD;*/

		CGameMain(void);
		~CGameMain(void);

		INT Run(const CHAR* pcPath);

	private:
		CGameInputHandler m_InputHandler;

		CGameTransversePointOutputHandler m_TransversePointOutputHandler;
		CGameVerticalPointOutputHandler m_VerticalPointOutputHandler;
		CGameScalePointOutputHandler m_ScalePointOutputHandler;
		CGameClickPointOutputHandler m_ClickPointOutputHandler;

		CSceneManager* m_pSceneManager;

		CInput m_Input;
		CPointOutput m_PointOutput;
		CTower m_Tower;
	};
}