package alchemy.astep.engineres
{
	import alchemy.astep.utility.Utility;
	import alchemy.resource.extend.CMaterial;

	public class MaterialRes extends BasicRes
	{
		static public const SOURCE:uint 		= 0;
		static public const FIRENEW:uint 		= 1;
		
		public var daeSource:String;
		
		public function MaterialRes( createType:uint, sourceMaterial:CMaterial = null )
		{
			_materialType = createType;
			
			if(_materialType == FIRENEW)
			{
				_ambientColor = new Vector.<Number>(4,true);
				_diffuseColor = new Vector.<Number>(4,true);
				_specularColor = new Vector.<Number>(4,true);
				_emissiveColor = new Vector.<Number>(4,true);
			}
			
			_power = -1.0;
			_textureRes = new Vector.<TextureRes>;
		}
		
		public function set material( val:CMaterial ):void
		{
			Utility.assert(val != null, "error argument material!");
			_material = val;
			var i:uint, len:uint;
			if(_materialType == SOURCE)
			{
				if(_ambientColor != null)
				{
					_material.ambient.redMultiplier   = _ambientColor[0];
					_material.ambient.greenMultiplier = _ambientColor[1];
					_material.ambient.blueMultiplier  = _ambientColor[2];
					_material.ambient.alphaMultiplier = _ambientColor[3];
				}
				
				if(_diffuseColor != null)
				{
					_material.diffuse.redMultiplier   = _diffuseColor[0];
					_material.diffuse.greenMultiplier = _diffuseColor[1];
					_material.diffuse.blueMultiplier  = _diffuseColor[2];
					_material.diffuse.alphaMultiplier = _diffuseColor[3];
				}
				
				if(_specularColor != null)
				{
					_material.specular.redMultiplier   = _specularColor[0];
					_material.specular.greenMultiplier = _specularColor[1];
					_material.specular.blueMultiplier  = _specularColor[2];
					_material.specular.alphaMultiplier = _specularColor[3];
				}
				
				if(_emissiveColor != null)
				{
					_material.emissive.redMultiplier   = _emissiveColor[0];
					_material.emissive.greenMultiplier = _emissiveColor[1];
					_material.emissive.blueMultiplier  = _emissiveColor[2];
					_material.emissive.alphaMultiplier = _emissiveColor[3];
				}
				
				if(_power != -1.0)
					_material.power = _power;
				
				len = _textureRes.length;
				for(i=0; i<len; ++i)
				{
					if(_textureRes[i].texture != null)
						_material.addTexture( _textureRes[i].texture );
				}
			}
			else
			{
				_material.ambient.redMultiplier   = _ambientColor[0];
				_material.ambient.greenMultiplier = _ambientColor[1];
				_material.ambient.blueMultiplier  = _ambientColor[2];
				_material.ambient.alphaMultiplier = _ambientColor[3];
				
				_material.diffuse.redMultiplier   = _diffuseColor[0];
				_material.diffuse.greenMultiplier = _diffuseColor[1];
				_material.diffuse.blueMultiplier  = _diffuseColor[2];
				_material.diffuse.alphaMultiplier = _diffuseColor[3];
				
				_material.specular.redMultiplier   = _specularColor[0];
				_material.specular.greenMultiplier = _specularColor[1];
				_material.specular.blueMultiplier  = _specularColor[2];
				_material.specular.alphaMultiplier = _specularColor[3];
				
				_material.emissive.redMultiplier   = _emissiveColor[0];
				_material.emissive.greenMultiplier = _emissiveColor[1];
				_material.emissive.blueMultiplier  = _emissiveColor[2];
				_material.emissive.alphaMultiplier = _emissiveColor[3];
				
				_material.power = _power;
				
				len = _textureRes.length;
				for(i=0; i<len; ++i)
				{
					if(_textureRes[i].texture != null)
						_material.addTexture( _textureRes[i].texture );
				}
			}
		}
		
		public function set ambientColor( val:Vector.<Number> ):void
		{
			if(_material != null)
			{
				if(!_ambientColor)
					_ambientColor = new Vector.<Number>(4,true);
				
				for(var i:uint=0; i<4; ++i)
					_ambientColor[i] = val[i];
				
				if(_material)
				{
					_material.ambient.redMultiplier   = _ambientColor[0];
					_material.ambient.greenMultiplier = _ambientColor[1];
					_material.ambient.blueMultiplier  = _ambientColor[2];
					_material.ambient.alphaMultiplier = _ambientColor[3];
				}
			}
		}
		
		public function get ambientColor():Vector.<Number>
		{
			return _ambientColor ? ( new Vector.<Number>( [_ambientColor[0],  _ambientColor[1], _ambientColor[2], _ambientColor[3]] ) ) : null;
		}
		
		public function set diffuseColor( val:Vector.<Number> ):void
		{
			if(_material != null)
			{
				if(!_diffuseColor)
					_diffuseColor = new Vector.<Number>(4,true);
				
				for(var i:uint=0; i<4; ++i)
					_diffuseColor[i] = val[i];
				
				if(_material)
				{
					_material.diffuse.redMultiplier   = _diffuseColor[0];
					_material.diffuse.greenMultiplier = _diffuseColor[1];
					_material.diffuse.blueMultiplier  = _diffuseColor[2];
					_material.diffuse.alphaMultiplier = _diffuseColor[3];
				}
			}
		}
		
		public function get diffuseColor():Vector.<Number>
		{
			return _diffuseColor ? ( new Vector.<Number>( [_diffuseColor[0],  _diffuseColor[1], _diffuseColor[2], _diffuseColor[3]] ) ) : null;
		}
		
		public function set specularColor( val:Vector.<Number> ):void
		{
			if(_material != null)
			{
				if(!_specularColor)
					_specularColor = new Vector.<Number>(4,true);
				
				for(var i:uint=0; i<4; ++i)
					_specularColor[i] = val[i];
				
				if(_material)
				{
					_material.specular.redMultiplier   = _specularColor[0];
					_material.specular.greenMultiplier = _specularColor[1];
					_material.specular.blueMultiplier  = _specularColor[2];
					_material.specular.alphaMultiplier = _specularColor[3];
				}
			}
		}
		
		public function get specularColor():Vector.<Number>
		{
			return _specularColor ? ( new Vector.<Number>( [_specularColor[0],  _specularColor[1], _specularColor[2], _specularColor[3]] ) ) : null;
		}
		
		public function set emissiveColor( val:Vector.<Number> ):void
		{
			if(_material != null)
			{
				if(!_emissiveColor)
					_emissiveColor = new Vector.<Number>(4,true);
				
				for(var i:uint=0; i<4; ++i)
					_ambientColor[i] = val[i];
				
				if(_material)
				{
					_material.emissive.redMultiplier   = _emissiveColor[0];
					_material.emissive.greenMultiplier = _emissiveColor[1];
					_material.emissive.blueMultiplier  = _emissiveColor[2];
					_material.emissive.alphaMultiplier = _emissiveColor[3];
				}
			}
		}
		
		public function get emissiveColor():Vector.<Number>
		{
			return _emissiveColor ? ( new Vector.<Number>( [ _emissiveColor[0],  _emissiveColor[1], _emissiveColor[2], _emissiveColor[3] ] ) ) : null;
		}
		
		public function set power( val:Number ):void
		{
			_power = val;
			if(_material)
				_material.power = val;
		}
		
		public function get power():Number
		{
			return _power;
		}
		
		override public function get type():String
		{
			return RES_TYPE_MATERIAL;
		}
		
		public function get materialType():uint
		{
			return _materialType;
		}
		
		public function get material():CMaterial
		{
			return _material;
		}
		
		public function addTextureRes( textureRes:TextureRes ):void
		{
			if(!textureRes)
				return;
			
			_textureRes.push(textureRes);
			if(_material && textureRes.texture)
				_material.addTexture( textureRes.texture );
		}
		
		public function getTextureRes( index:uint ):TextureRes
		{
			return index < _textureRes.length ? _textureRes[index] : null;
		}
		
		public function get textureNum():uint
		{
			return _textureRes.length;
		}
		
		private var _material:CMaterial;
		private var _materialType:uint;
		private var _textureRes:Vector.<TextureRes>;
		
		private var _ambientColor:Vector.<Number>;
		private var _diffuseColor:Vector.<Number>;
		private var _specularColor:Vector.<Number>;
		private var _emissiveColor:Vector.<Number>;
		private var _power:Number;
	}
}