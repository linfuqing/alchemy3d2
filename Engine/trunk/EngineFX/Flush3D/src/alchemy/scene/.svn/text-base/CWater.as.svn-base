package alchemy.scene
{
	import alchemy.core.CLibrary;
	import alchemy.data.CEffectInfoData;
	import alchemy.manager.CMeshManager;

	public class CWater extends CSceneNode
	{
		public function CWater(width:Number,
							   height:Number,
							   widthSegment:uint = 1,
							   heightSegment:uint = 1,
							   reflectionMapShift:int = 0, 
							   refractionMapShift:int = 0,
							   normalMap:uint = 0,
							   material:uint = 0,
							   effectInfos:Vector.<CEffectInfoData> = null)
		{
			_width               = width;
			_height              = height;
			_widthSegment        = widthSegment;
			_heightSegment       = heightSegment;
			_reflectionMapShift  = reflectionMapShift;
			_refractionMapShift  = refractionMapShift;
			_normalMap           = normalMap;
			_material            = material;
			_effectInfos         = effectInfos;
			
			super(0, 0, 0);
		}
		
		override protected function getSceneNodePointers():Array
		{
			CMeshManager.instance;
			
			var effectInfosPointer:uint = 0;
			var effectInfosLength:uint = 0;
			if(_effectInfos)
			{
				effectInfosLength  = _effectInfos.length;
				effectInfosPointer = CLibrary.method.CreateBuffer(effectInfosLength * CEffectInfoData.SIZE);
				var effectInfo:CEffectInfoData;
				CLibrary.memory.position = effectInfosPointer;
				for(var i:uint = 0; i < effectInfosLength; ++ i)
				{
					effectInfo = _effectInfos[i];
					CLibrary.memory.writeUnsignedInt(effectInfo.effectInfo.pointer);
					
					if(effectInfo.blendMethod)
						CLibrary.memory.writeUnsignedInt(effectInfo.blendMethod.pointer);
					else
						CLibrary.memory.writeUnsignedInt(0);
					
					CLibrary.memory.writeUnsignedInt(0);
					CLibrary.memory.writeUnsignedInt(0);
					CLibrary.memory.writeUnsignedInt(0);
				}
			}
			
			var pointers:Array = CLibrary.method.CreateWater(_width, _height, _widthSegment, _heightSegment, _reflectionMapShift, _refractionMapShift, _normalMap, _material, effectInfosPointer, effectInfosLength);
			
			if(effectInfosPointer)
				CLibrary.method.DestroyBuffer(effectInfosPointer);
			
			return pointers;
		}
		
		private var _width:Number;
		private var _height:Number;
		private var _widthSegment:uint;
		private var _heightSegment:uint;
		private var _reflectionMapShift:int;
		private var _refractionMapShift:int;
		private var _normalMap:uint;
		private var _material:uint;
		private var _effectInfos:Vector.<CEffectInfoData>;
	}
}