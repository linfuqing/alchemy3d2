package alchemy.data.rendermethod
{
	import alchemy.core.CPointer;
	import alchemy.data.CSafeData;
	import alchemy.tools.CDebug;
	
	public class CParameterListData extends CSafeData
	{
		public static const LABEL_PARAMETER_POSITION_OFFSET          :int = 0;
		public static const LABEL_PARAMETER_UV_OFFSET                :int = 1;
			
		public static const LABEL_PARAMETER_ELAPSED_TIME             :int = 2;
		public static const LABEL_PARAMETER_TOTAL_TIME               :int = 3;
		
		public static const LABEL_PARAMETER_AMBIENT_LIGHT            :int = 4;
		
		public static const LABEL_PARAMETER_WORLD_MATRIX             :int = 5;
		public static const LABEL_PARAMETER_INVERSE_WORLD_MATRIX     :int = 6;
		
		public static const TOTAL_PARAMETER_LABEL                    :int = 7;
		
		public static const LABEL_BONE_MATRIX_ARRAY                  :int = 0;
			
		public static const TOTAL_ARRAY_LABEL                        :int = 1;           
			
		/*public static const LABEL_MATRIX_WORLD                       :int = 0;
		public static const LABEL_MATRIX_VIEW                        :int = 1;
		public static const LABEL_MATRIX_PROJECTION                  :int = 2;
			
		public static const LABEL_MATRIX_WORLD_VIEW                  :int = 3;
		public static const LABEL_MATRIX_VIEW_PROJECTION             :int = 4;
		public static const LABEL_MATRIX_WORLD_VIEW_PROJECTION       :int = 5;
		public static const LABEL_INVERSE_WORLD_MATRIX               :int = 6;
		public static const LABEL_INVERSE_VIEW_MATRIX                :int = 7;
		public static const LABEL_INVERSE_PROJECTION_MATRIX          :int = 8;
		
		public static const TOTAL_MATRIX_LABEL                       :int = 9;*/
			
			
		public static const LABEL_CAMERA_POSITION                    :int = 0;
		public static const LABEL_CAMERA_DIRECTION                   :int = 1;
		public static const LABEL_CAMERA_DIRECTION_NEGATE            :int = 2;
		public static const LABEL_CAMERA_NEAR                        :int = 3;
		public static const LABEL_CAMERA_FAR                         :int = 4;
		public static const LABEL_CAMERA_DISTANCE                    :int = 5;
		public static const LABEL_CAMERA_LOCAL_POSITION              :int = 6;
		public static const LABEL_CAMERA_LOCAL_DIRECTION             :int = 7;
		public static const LABEL_CAMERA_LOCAL_DIRECTION_NEGATE      :int = 8;
		
		public static const LABEL_CAMERA_VIEW_MATRIX                 :int = 9;
		public static const LABEL_CAMERA_PROJECTION_MATRIX           :int = 10;
		
		public static const LABEL_CAMERA_WORLD_VIEW_MATRIX           :int = 11;
		public static const LABEL_CAMERA_VIEW_PROJECTION_MATRIX      :int = 12;
		public static const LABEL_CAMERA_WORLD_VIEW_PROJECTION_MATRIX:int = 13;
		public static const LABEL_CAMERA_INVERSE_VIEW_MATRIX         :int = 14;
		public static const LABEL_CAMERA_INVERSE_PROJECTION_MATRIX   :int = 15;
		
		public static const TOTAL_CAMERA_LABEL                       :int = 16;
			
			
		public static const LABEL_MATERIAL_AMBIENT                   :int = 0;
		public static const LABEL_MATERIAL_DIFFUSE                   :int = 1;
		public static const LABEL_MATERIAL_SPECULAR                  :int = 2;
		public static const LABEL_MATERIAL_EMISSIVE                  :int = 3;
		public static const LABEL_MATERIAL_POWER                     :int = 4;
		public static const LABEL_MATERIAL_INVERSE_255_POWER         :int = 5;
		
		public static const TOTAL_MATERIAL_LABEL                     :int = 6;
			
			
		public static const LABEL_LIGHT_TYPE                         :int = 0;
		public static const LABEL_LIGHT_AMBIENT                      :int = 1;
		public static const LABEL_LIGHT_DIFFUSE                      :int = 2;
		public static const LABEL_LIGHT_SPECULAR                     :int = 3;
		public static const LABEL_LIGHT_POSITION                     :int = 4;
		public static const LABEL_LIGHT_DIRECTION                    :int = 5;
		public static const LABEL_LIGHT_RANGE                        :int = 6;
		public static const LABEL_LIGHT_FALLOFF                      :int = 7;
		public static const LABEL_LIGHT_ATTENUATION0                 :int = 8;
		public static const LABEL_LIGHT_ATTENUATION1                 :int = 9;
		public static const LABEL_LIGHT_ATTENUATION2                 :int = 10;
		public static const LABEL_LIGHT_THETA                        :int = 11;
		public static const LABEL_LIGHT_PHI                          :int = 12;
		public static const LABEL_LIGHT_COS_THETA                    :int = 13;
		public static const LABEL_LIGHT_INVERSE_COS_THETA_SUBSTRUCT_COS_PHI:int = 14;
		public static const LABEL_LIGHT_LOCAL_POSITION               :int = 15;
		public static const LABEL_LIGHT_LOCAL_DIRECTION              :int = 16;
		public static const LABEL_LIGHT_LOCAL_DIRECTION_NEGATE       :int = 17
		public static const LABEL_LIGHT_WORLD_VIEW_PROJECTION_MATRIX :int = 18;
		
		public static const TOTAL_LIGHT_LABEL                        :int = 19;
		
		public static const LABEL_CONSTANT_ZERO                      :int = 0;
		public static const LABEL_CONSTANT_ONE                       :int = 1;
		public static const LABEL_CONSTANT_HALF_ONE                  :int = 2;
		public static const LABEL_CONSTANT_TWO                       :int = 3;
		public static const LABEL_CONSTANT_FOUR                      :int = 4;
		public static const LABEL_CONSTANT_255                       :int = 5;
		public static const LABEL_CONSTANT_INVERSE_255               :int = 6;
		public static const LABEL_CONSTANT_FLOAT_TO_RGBA_CODE        :int = 7;
		public static const LABEL_CONSTANT_RGB_TO_FLOAT_CODE         :int = 8;
		
		public static const TOTAL_CONSTANT_LABEL                     :int = 9;
		
		public static const LABEL_TEXTURE_WIDTH                      :int = 0;
		public static const LABEL_TEXTURE_HEIGHT                     :int = 1;
		
		public static const TOTAL_TEXTURE_LABEL                      :int = 2;
		
		public function get parameters():Vector.<CParameterInfoData>
		{
			return _parameters;
		}
		
		public function get arrayParameters():Vector.<CParameterInfoData>
		{
			return _arrayParameters;
		}
		
		/*public function get matrixParameters():Vector.<CParameterInfoData>
		{
			return _matrixParameters;
		}*/
		
		/*public function get cameraParameters():Vector.<CParameterInfoData>
		{
			return _cameraParameters;
		}*/
		
		public function get materialParameters():Vector.<CParameterInfoData>
		{
			return _materialParameters;
		}
		
		public function get constantParameters():Vector.<CParameterInfoData>
		{
			return _constantParameters;
		}
		
		public function get cameraParameters():Vector.<Vector.<CParameterInfoData>>
		{
			return _cameraParameters.concat();
		}
		
		public function get lightParameters():Vector.<Vector.<CParameterInfoData>>
		{
			return _lightParameters.concat();
		}
		
		public function get textureParameters():Vector.<Vector.<CParameterInfoData>>
		{
			return _textureParameters.concat();
		}
		
		public function get textures():Vector.<CParameterInfoData>
		{
			return _textures;
		}
		
		/*public function get vertexElementParameters():Vector.<CVertexElementParameterData>
		{
			return _vertexElementParameters.concat();
		}*/
		
		public function CParameterListData()
		{
			super();
			
			init();
		}
		
		public function addLightParameters(parameters:Vector.<CParameterInfoData>):void
		{
			if(!parameters)
				return;
			
			var length:uint = Math.min(parameters.length, TOTAL_LIGHT_LABEL);
			var newLightParameters:Vector.<CParameterInfoData> = new Vector.<CParameterInfoData>(TOTAL_LIGHT_LABEL , true);
			for(var i:uint = 0; i < length; ++ i)
				newLightParameters[i] = parameters[i];
			
			_lightParameters.push(newLightParameters);
		}
		
		public function setLightParameters(parameters:Vector.<CParameterInfoData>, index:uint):Boolean
		{
			if(!parameters)
				return false;
			
			if(index < _lightParameters.length)
			{
				var length:uint = Math.min(parameters.length, TOTAL_LIGHT_LABEL);
				var newLightParameters:Vector.<CParameterInfoData> = new Vector.<CParameterInfoData>(TOTAL_LIGHT_LABEL , true);
				for(var i:uint = 0; i < length; ++ i)
					newLightParameters[i] = parameters[i];
				
				_lightParameters[index] = newLightParameters;
				
				return true;
			}

			return false;
		}
		
		public function addTextureParameters(texture:CParameterInfoData, parameters:Vector.<CParameterInfoData> = null):void
		{
			if(!texture)
				return;
			
			var textureParameters:Vector.<CParameterInfoData> = null;
			if(parameters)
			{
				var length:uint = Math.min(parameters.length, TOTAL_TEXTURE_LABEL);
				textureParameters = new Vector.<CParameterInfoData>(TOTAL_TEXTURE_LABEL , true);
				for(var i:uint = 0; i < length; ++ i)
					textureParameters[i] = parameters[i];
			}
			
			_textureParameters.push(textureParameters);
			_textures.push(texture);
		}
		
		public function setTextureParameters(texture:CParameterInfoData, index:uint, parameters:Vector.<CParameterInfoData> = null):Boolean
		{
			if(!texture)
				return false;
			
			if(index < _textureParameters.length)
			{
				var textureParameters:Vector.<CParameterInfoData> = null;
				if(parameters)
				{
					var length:uint = Math.min(parameters.length, TOTAL_TEXTURE_LABEL);
					textureParameters = new Vector.<CParameterInfoData>(TOTAL_TEXTURE_LABEL , true);
					for(var i:uint = 0; i < length; ++ i)
						textureParameters[i] = parameters[i];
				}
				
				_textureParameters[index] = textureParameters;
				_textures[index]          = texture;
				
				return true;
			}
			
			return false;
		}
		
		private function init():void
		{
			_parameters              = new Vector.<CParameterInfoData>(TOTAL_PARAMETER_LABEL, true);
			_arrayParameters         = new Vector.<CParameterInfoData>(TOTAL_ARRAY_LABEL    , true);
			//_matrixParameters        = new Vector.<CParameterInfoData>(TOTAL_MATRIX_LABEL   , true);
			//_cameraParameters        = new Vector.<CParameterInfoData>(TOTAL_CAMERA_LABEL   , true);
			_materialParameters      = new Vector.<CParameterInfoData>(TOTAL_MATERIAL_LABEL , true);
			_constantParameters      = new Vector.<CParameterInfoData>(TOTAL_CONSTANT_LABEL , true);
			
			_cameraParameters        = new Vector.<Vector.<CParameterInfoData>>();
			_lightParameters         = new Vector.<Vector.<CParameterInfoData>>();
			_textureParameters       = new Vector.<Vector.<CParameterInfoData>>();
			
			_textures                = new Vector.<CParameterInfoData>();
			
			//_vertexElementParameters = new Vector.<CVertexElementParameterData>();
		}
		
		private var _parameters:Vector.<CParameterInfoData>;
		private var _arrayParameters:Vector.<CParameterInfoData>;
		//private var _matrixParameters:Vector.<CParameterInfoData>;
		//private var _cameraParameters:Vector.<CParameterInfoData>;
		private var _materialParameters:Vector.<CParameterInfoData>;
		private var _constantParameters:Vector.<CParameterInfoData>;
		
		private var _cameraParameters:Vector.<Vector.<CParameterInfoData>>;
		private var _lightParameters:Vector.<Vector.<CParameterInfoData>>;
		private var _textureParameters:Vector.<Vector.<CParameterInfoData>>;
		
		private var _textures:Vector.<CParameterInfoData>;
		
		//private var _vertexElementParameters:Vector.<CVertexElementParameterData>;
	}
}