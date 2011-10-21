#pragma once

namespace alchemy
{
	class CSceneNode;
	class CSceneNodeInterface
	{
	public:

		CSceneNodeInterface(void)
		{
		}

		virtual ~CSceneNodeInterface(void)
		{
		};

		virtual bool AddChild(CSceneNode& Child) = 0;
		virtual bool RemoveChild(CSceneNode& Child) = 0;
	};
}