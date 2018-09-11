package alchemy.data.effect
{
	import alchemy.data.CSafeData;
	
	public class CSamplerDeclarationData extends CSafeData
	{
		static public const TEXTURE2D_TYPE:uint 				= 0;
		static public const TEXTURE3D_TYPE:uint 				= TEXTURE2D_TYPE + 1;
		static public const TEXTURE_CUBE_TYPE:uint 				= TEXTURE3D_TYPE + 1;
		
		static public const NONE_FILTER:uint 					= 0;
		static public const POINT_FILTER:uint 					= NONE_FILTER + 1;
		static public const LINEAR_FILTER:uint 					= POINT_FILTER + 1;
		//static public const ANISOTROPIC_FILTER:uint 			= LINEAR_FILTER + 1;
		//static public const PYRAMIDALQUAD_FILTER:uint 			= ANISOTROPIC_FILTER + 1;
		//static public const GAUSSIANQUAD_FILTER:uint 			= PYRAMIDALQUAD_FILTER + 1;
		//static public const CONVOLUTIONMONO_FILTER:uint 		= GAUSSIANQUAD_FILTER + 1;
		
		static public const WRAP_ADDRESS_MODE:uint 				= 0;
		static public const MIRROR_ADDRESS_MODE:uint 			= WRAP_ADDRESS_MODE + 1;
		static public const CLAMP_ADDRESS_MODE:uint 			= MIRROR_ADDRESS_MODE + 1;
		static public const BORDER_ADDRESS_MODE:uint 			= CLAMP_ADDRESS_MODE + 1;
		static public const MIRROR_ONCE_ADDRESS_MODE:uint 		= BORDER_ADDRESS_MODE + 1;
		
		public var type:uint;
		public var minFilter:uint;
		public var magFilter:uint;
		public var mipFilter:uint;
		public var addressU:uint;
		public var addressV:uint;
		public var addressW:uint;
		
		public function CSamplerDeclarationData()
		{
			
		}
	}
}