package alchemy.effect.postprocess
{
	import alchemy.core.CLibrary;
	import alchemy.scene.CSceneNode;
	import alchemy.tools.CDebug;

	public class CVolumeLightShow extends CAbstractPostProcessEffect
	{
		public function CVolumeLightShow(occTexture:uint, lightIndex:uint = 0, numSamples:uint = 3,
												desity:Number = 1.0, exposure:Number = 1.0, decay:Number = 0.8, scatterWeight:Number = 0.8)
		{
			var ptrs:Array = CLibrary.method.CreateVolumeLightShow(occTexture, lightIndex, numSamples,
				desity, exposure, decay, scatterWeight);
			
			CDebug.assert( ptrs != null, "CVolumeLightShow::CVolumeLightShow() - CreateVolumeLightShow - error!" );
			
			super( ptrs[0], ptrs[1] );
			
			_objectInterfacePointer = ptrs[2];
			
			//ptrs = CLibrary.method.InitObject(_objectInterfacePointer);
			
			//CDebug.assert( ptrs != null, "CVolumeLightShow::CVolumeLightShow() - InitObject - error!" );
			
//			_desitysPointer 	= ptrs[0];
//			_numSamplesPointer 	= ptrs[1];
//			_exposurePointer 	= ptrs[2];
//			_decayPointer 		= ptrs[3];
//			_scatterWeightPointer 	= ptrs[4];
		}
		
		public function set desitysVal( val:Number ):void
		{
			if(_desitysPointer)
			{
				CLibrary.memory.position = _desitysPointer;
				CLibrary.memory.writeFloat(val);
			}
		}
		
		public function get desitysVal():Number
		{
			if(_desitysPointer)
			{
				CLibrary.memory.position = _desitysPointer;
				return CLibrary.memory.readFloat();
			}
			
			return 0.0;
		}
		
		public function set exposureVal( val:Number ):void
		{
			if(_exposurePointer)
			{
				CLibrary.memory.position = _exposurePointer;
				CLibrary.memory.writeFloat(val);
			}
		}
		
		public function get exposureVal():Number
		{
			if(_exposurePointer)
			{
				CLibrary.memory.position = _exposurePointer;
				return CLibrary.memory.readFloat();
			}
			
			return 0.0;
		}
		
		public function set decayVal( val:Number ):void
		{
			if(_decayPointer)
			{
				CLibrary.memory.position = _decayPointer;
				CLibrary.memory.writeFloat(val);
			}
		}
		
		public function get decayVal():Number
		{
			if(_decayPointer)
			{
				CLibrary.memory.position = _decayPointer;
				return CLibrary.memory.readFloat();
			}
			
			return 0.0;
		}
		
		public function set scatterWeightVal( val:Number ):void
		{
			if(_scatterWeightPointer)
			{
				CLibrary.memory.position = _scatterWeightPointer;
				CLibrary.memory.writeFloat(val);
			}
		}
		
		public function get scatterWeightVal():Number
		{
			if(_scatterWeightPointer)
			{
				CLibrary.memory.position = _scatterWeightPointer;
				return CLibrary.memory.readFloat();
			}
			
			return 0.0;
		}
		
		private var _objectInterfacePointer:uint;
		private var _desitysPointer:uint;
		private var _exposurePointer:uint;
		private var _decayPointer:uint;
		private var _scatterWeightPointer:uint;
		
		private var _numSamplesPointer:uint;
	}
}