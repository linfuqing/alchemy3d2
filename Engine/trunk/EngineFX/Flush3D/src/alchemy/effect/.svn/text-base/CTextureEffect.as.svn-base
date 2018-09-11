package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.data.effect.CSamplerDeclarationData;

	public class CTextureEffect extends CAbstractEffectInfo
	{
		public function CTextureEffect(samplerIndex:uint = 0, TexcoordIndex:uint = 0,  samplerData:CSamplerDeclarationData = null)
		{
			var dataPtr:uint = 0;
			
			if(samplerData)
			{
				dataPtr = CLibrary.method.CreateBuffer( CLibrary.INT_SIZE * 7 );
				CLibrary.memory.position = dataPtr;
				CLibrary.memory.writeUnsignedInt(samplerData.type);
				CLibrary.memory.writeUnsignedInt(samplerData.minFilter);
				CLibrary.memory.writeUnsignedInt(samplerData.magFilter);
				CLibrary.memory.writeUnsignedInt(samplerData.mipFilter);
				CLibrary.memory.writeUnsignedInt(samplerData.addressU);
				CLibrary.memory.writeUnsignedInt(samplerData.addressV);
				CLibrary.memory.writeUnsignedInt(samplerData.addressW);
			}
			
			var pointers:Array = CLibrary.method.CreateTextureEffect(samplerIndex, TexcoordIndex, dataPtr);
			
			super(pointers[0]); 
			
			_sourcePointer = pointers[1];
			
			if(samplerData && dataPtr)
				CLibrary.method.DestroyBuffer( dataPtr );
		}
		
		private var _sourcePointer:uint;
	}
}