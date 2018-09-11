package alchemy.astep.utility
{
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.data.CTextureData;
	import alchemy.manager.CMeshManager;
	import alchemy.manager.CRenderSystem;
	import alchemy.manager.CSceneManager;
	import alchemy.resource.basic.CTexture;
	
	import flash.utils.Dictionary;

	public class Utility
	{
		public function Utility()
		{
			
		}
		
		static public function waring( msg:String ):void
		{
			trace("warning: " + msg);
		}
		
		static public function assert(exce:Boolean, msg:String):void
		{
			if(!exce)
				throw( new Error("error : " + msg) );
		}
		
		static private var _textureDataPool:Dictionary = new Dictionary;
		static public function createTexture( renderSystem:CRenderSystem, texDataHandle:uint, forceCreate:Boolean = false ):CTexture
		{
			if(!renderSystem || !texDataHandle)
				return null;
			
			var tex:CTexture;
			var texData:CTextureData = CMeshManager.instance.getTextureData(texDataHandle);
			
			if(forceCreate)
				tex = renderSystem.createTextureWithData(texData);
			else
			{
				tex = _textureDataPool[texDataHandle];
				tex = tex ? tex : renderSystem.createTextureWithData(texData);
			}
			
			_textureDataPool[texDataHandle] = tex;
			
			return tex;
		}
		
		static public function getAppropriateXML( datas:XMLList, value:String ):XML
		{
			for each(var data:XML in datas)
			{
				if( data.@name == value )
					return data;
			}
			
			return null;
		}
	}
}