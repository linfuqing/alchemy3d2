package alchemy.astep.engineres
{
	import alchemy.resource.basic.CTexture;

	public class TextureRes extends BasicRes
	{
		public function TextureRes()
		{
			
		}
		
		public function build( val:String, texture:CTexture = null ):void
		{
			_url = val;
			_texture = texture;
		}
		
		public function get url():String
		{
			return _url;
		}
		
		public function set texture( val:CTexture ):void
		{
			_texture = val;
		}
		
		public function get texture():CTexture
		{
			return _texture;
		}
		
		override public function get type():String
		{
			return RES_TYPE_TEXTURE;
		}
		
		private var _url:String;
		private var _texture:CTexture;
	}
}