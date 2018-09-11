package alchemy.data
{
	import flash.utils.ByteArray;
	import flash.utils.IDataInput;
	import flash.utils.IDataOutput;
	
	import spark.components.supportClasses.Range;
	
	public final class CDataAccessor implements IDataInput, IDataOutput
	{
		private var _isSelfMem:Boolean = false;
		
		private var _curBytes:ByteArray;
		private var _length:uint;
		private var _headPos:uint;
		private var _cursor:uint;
		
		public function CDataAccessor(mem:ByteArray = null,head:uint = 0,len:uint = 0)
		{
			if(mem == null)
			{
				_isSelfMem = true;
				_curBytes = new ByteArray();
				_curBytes.length = this.length = len;
			}
			//this.headpos = head;
			this.cursor = 0;
		}
		
		public function set headPos(pos:uint):void
		{
			if(_curBytes == null || pos >= _curBytes.length)
				throw(new RangeError);
			
			this.cursor = 0;
			_headPos = pos;
		}
		public function get headPos():uint
		{
			return _headPos;
		}
		
		public function set cursor(val:uint):void
		{
			if(_curBytes == null || val >= length)
				throw(new RangeError);

			_cursor = val;
		}
		public function get cursor():uint
		{
			return _cursor;
		}
		
		public function set length(len:uint):void
		{
//			if(_curBytes == null)
//				throw(new NullAgumentError);
//			
//			if(_isSelfMem == true)
//				_curBytes.length = _length = len;
//			else
//			{
//				if( (this.headPos + val) >= _curBytes.length)
//					throw(new RangeError);
//				_length = len;
//			}
		}
		
		public function get length():uint
		{
			return _length;
		}
		
		public function readBytes(bytes:ByteArray, offset:uint=0, length:uint=0):void
		{
			var posOld:uint, posNew:uint;
			posOld = _curBytes.position;
			
			_curBytes.readBytes(bytes, offset, length);
			
			posNew = _curBytes.position;
			
			this.cursor += posNew - posOld;
		}
		
		public function readBoolean():Boolean
		{
			return _curBytes.readBoolean();
		}
		
		public function readByte():int
		{
			return 0;
		}
		
		public function readUnsignedByte():uint
		{
			return 0;
		}
		
		public function readShort():int
		{
			return 0;
		}
		
		public function readUnsignedShort():uint
		{
			return 0;
		}
		
		public function readInt():int
		{
			return 0;
		}
		
		public function readUnsignedInt():uint
		{
			return 0;
		}
		
		public function readFloat():Number
		{
			return 0;
		}
		
		public function readDouble():Number
		{
			return 0;
		}
		
		public function readMultiByte(length:uint, charSet:String):String
		{
			return null;
		}
		
		public function readUTF():String
		{
			return null;
		}
		
		public function readUTFBytes(length:uint):String
		{
			return null;
		}
		
		public function get bytesAvailable():uint
		{
			return 0;
		}
		
		public function readObject():*
		{
			return null;
		}
		
		public function get objectEncoding():uint
		{
			return 0;
		}
		
		public function set objectEncoding(version:uint):void
		{
		}
		
		public function get endian():String
		{
			return null;
		}
		
		public function set endian(type:String):void
		{
		}
		
		public function writeBytes(bytes:ByteArray, offset:uint=0, length:uint=0):void
		{
		}
		
		public function writeBoolean(value:Boolean):void
		{
		}
		
		public function writeByte(value:int):void
		{
		}
		
		public function writeShort(value:int):void
		{
		}
		
		public function writeInt(value:int):void
		{
		}
		
		public function writeUnsignedInt(value:uint):void
		{
		}
		
		public function writeFloat(value:Number):void
		{
		}
		
		public function writeDouble(value:Number):void
		{
		}
		
		public function writeMultiByte(value:String, charSet:String):void
		{
		}
		
		public function writeUTF(value:String):void
		{
		}
		
		public function writeUTFBytes(value:String):void
		{
		}
		
		public function writeObject(object:*):void
		{
		}
	}
}