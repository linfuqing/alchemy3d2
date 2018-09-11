package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.data.effect.CSamplerDeclarationData;
	import alchemy.tools.CDebug;
	
	import flash.geom.Point;

	public class CTextureBlurEffect extends CAbstractEffectInfo
	{
		public function CTextureBlurEffect( uvOffsets:Vector.<Point>, uvWeights:Vector.<Number>, samplerIndex:uint = 0,
					uTexCoordIndex:uint = 0, samplerData:CSamplerDeclarationData = null)
		{
			CDebug.assert( uvOffsets && uvWeights && uvWeights.length == uvOffsets.length, "CTextureBlurEffect() - Error!");
			
			_samplerAttributeLen = uvWeights.length;
			
			_samplerOffsetsPtr = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE * uvOffsets.length * 2 );
			_samplerWeightsPtr = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE * uvWeights.length );
			
			var i:uint, samplerDataPtr:uint = 0;
			
			if(samplerData)
			{
				samplerDataPtr = CLibrary.method.CreateBuffer( CLibrary.INT_SIZE * 7 );
				CLibrary.memory.position = samplerDataPtr;
				CLibrary.memory.writeUnsignedInt(samplerData.type);
				CLibrary.memory.writeUnsignedInt(samplerData.minFilter);
				CLibrary.memory.writeUnsignedInt(samplerData.magFilter);
				CLibrary.memory.writeUnsignedInt(samplerData.mipFilter);
				CLibrary.memory.writeUnsignedInt(samplerData.addressU);
				CLibrary.memory.writeUnsignedInt(samplerData.addressV);
				CLibrary.memory.writeUnsignedInt(samplerData.addressW);
			}
			
			CLibrary.memory.position = _samplerOffsetsPtr;
			for(i=0; i<_samplerAttributeLen; ++i)
			{
				CLibrary.memory.writeFloat( uvOffsets[i].x );
				CLibrary.memory.writeFloat( uvOffsets[i].y );
			}
			
			CLibrary.memory.position = _samplerWeightsPtr;
			for(i=0; i<_samplerAttributeLen; ++i)
				CLibrary.memory.writeFloat( uvWeights[i] );
			
			var ptrs:Array = CLibrary.method.CreateTextureBlurEffect(_samplerOffsetsPtr, _samplerWeightsPtr, samplerIndex, uTexCoordIndex, samplerDataPtr);
			
			CDebug.assert(ptrs != null, "CTextureBlurEffect() - CreateTextureBlurEffect() - error!");
			
			super( ptrs[0] );
			
			if(samplerDataPtr)
				CLibrary.method.DestroyBuffer(samplerDataPtr);
			
			_hostPtr = ptrs[1];
		}
		
		public function set samplerOffsets( val:Vector.<Point> ):void
		{
			if(_samplerOffsetsPtr && val && val.length >= _samplerAttributeLen)
			{
				CLibrary.memory.position = _samplerOffsetsPtr;
				for(var i:uint=0; i<_samplerAttributeLen; ++i)
				{
					CLibrary.memory.writeFloat( val[i].x );
					CLibrary.memory.writeFloat( val[i].y );
				}
			}
		}
		
		public function set samplerWeights( val:Vector.<Number> ):void
		{
			if(_samplerWeightsPtr && val && val.length >= _samplerAttributeLen)
			{
				CLibrary.memory.position = _samplerWeightsPtr;
				for(var i:uint=0; i<_samplerAttributeLen; ++i)
					CLibrary.memory.writeFloat( val[i] );
			}
		}
		
		private var _hostPtr:uint;
		private var _samplerAttributeLen:uint;
		private var _samplerOffsetsPtr:uint;
		private var _samplerWeightsPtr:uint;
	}
}