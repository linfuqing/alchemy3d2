package alchemy.resource.extend
{
	import alchemy.core.CLibrary;
	import alchemy.manager.CSceneManager;
	import alchemy.resource.basic.CTexture;
	
	import flash.geom.ColorTransform;
	
	public class CMaterial extends CHighLevelResource
	{
		//private var _textureFlag:uint;
		
		public function CMaterial(pointer:uint, handle:uint, interfacePointer:uint, sceneManager:CSceneManager)
		{
			super(pointer, handle, interfacePointer, sceneManager);
			
			init();
		}
		
		override public function destroy():void
		{
			
		}
		
		public function getTextureByIndex( index:uint ):uint
		{
			return _pointer ? CLibrary.method.GetTextureByIndexFromMaterial(_pointer, index) : 0;
		}
		
		public function get textureNum():uint
		{
			return _pointer ? CLibrary.method.GetTextureNumFromMaterial(_pointer) : 0;
		}
		
		public function set ambient(val:ColorTransform):void
		{
			CLibrary.memory.position = _ambientPointer;
			CLibrary.memory.writeFloat( val.alphaMultiplier );
			CLibrary.memory.writeFloat( val.redMultiplier );
			CLibrary.memory.writeFloat( val.greenMultiplier );
			CLibrary.memory.writeFloat( val.blueMultiplier );
		}
		
		public function get ambient():ColorTransform
		{
			CLibrary.memory.position = _ambientPointer;
			
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set diffuse(val:ColorTransform):void
		{
			CLibrary.memory.position = _diffusePointer;
			CLibrary.memory.writeFloat( val.redMultiplier );
			CLibrary.memory.writeFloat( val.greenMultiplier );
			CLibrary.memory.writeFloat( val.blueMultiplier );
		}
		
		public function get diffuse():ColorTransform
		{
			CLibrary.memory.position = _diffusePointer;
			
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set specular(val:ColorTransform):void
		{
			CLibrary.memory.position = _specularPointer;
			CLibrary.memory.writeFloat( val.redMultiplier );
			CLibrary.memory.writeFloat( val.greenMultiplier );
			CLibrary.memory.writeFloat( val.blueMultiplier );
		}
		
		public function get specular():ColorTransform
		{
			CLibrary.memory.position = _specularPointer;
			
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set emissive(val:ColorTransform):void
		{
			CLibrary.memory.position = _emissivePointer;
			CLibrary.memory.writeFloat( val.redMultiplier );
			CLibrary.memory.writeFloat( val.greenMultiplier );
			CLibrary.memory.writeFloat( val.blueMultiplier );
		}
		
		public function get emissive():ColorTransform
		{
			CLibrary.memory.position  = _emissivePointer;
			
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set power(val:Number):void
		{
			CLibrary.memory.position = _powerPointer;
			CLibrary.memory.writeFloat(val);
		}
		
		public function get power():Number
		{
			CLibrary.memory.position = _powerPointer;
			
			return CLibrary.memory.readFloat();
		}
		
		public function addTexture(texture:CTexture):void
		{
			if(texture == null)
				return;
			
//			var index:int = _textures.indexOf(texture);
//			if(index != -1)
//				return 0;
			
			//index = _textures.push(texture);
			
			CLibrary.method.AddTextureToMaterial(_pointer,texture.pointer);
		}
		
		public function removeTexture(texture:CTexture):void
		{
			if(texture == null)
				return;
			
			CLibrary.method.RemoveTextureFromMaterial(_pointer,texture.handle);
		}
		
		private function init():void
		{
			_color = new ColorTransform();
			
			var pointers:Array = getPointers();
			
			_ambientPointer 	= pointers[0];
			_diffusePointer 	= pointers[1];
			_specularPointer 	= pointers[2];
			_emissivePointer 	= pointers[3];
			_powerPointer 		= pointers[4];
		}
		
		private var _color:ColorTransform;
		
		private var _ambientPointer:uint;
		private var _diffusePointer:uint;
		private var _specularPointer:uint;
		private var _emissivePointer:uint;
		private var _powerPointer:uint;
	}
}