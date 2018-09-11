package alchemy.tools
{
	import alchemy.core.CLibrary;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;

	public class CMath
	{
		public static const maxinumBitTable:Vector.<uint> = Vector.<uint>([
			0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8]);
		
		public static const mininumBitTable:Vector.<uint> = Vector.<uint>([
			0, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			8, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1]);
		
		public static const RAD2DEG:Number = 180.0 / Math.PI;
		public static const DEG2RAD:Number = Math.PI / 180.0;
		
		public static function rad2deg(radian:Number):Number
		{
			return radian / Math.PI * 180.0;
		}
		
		public static function deg2rad(degree:Number):Number
		{
			return degree / 180 * Math.PI;
		}
		
		public static function leftMulTransformByteArray(transform:Matrix3D, rawValues:ByteArray, outValues:ByteArray, floatStride:int, isNormal:Boolean = false):void
		{
			var valuesOut:ByteArray = rawValues == outValues ? new ByteArray : outValues;
			valuesOut.endian = rawValues.endian;
			valuesOut.position = rawValues.position = 0;
			var matRaw:Vector.<Number> = transform.rawData;
			
			var len:int = rawValues.length/CLibrary.FLOAT_SIZE/floatStride;
			var pos:Vector3D = new Vector3D;
			var dstPos:Vector3D = new Vector3D;
			for(var i:int=0; i<len; ++i)
			{
				pos.x = rawValues.readFloat();
				pos.y = rawValues.readFloat();
				pos.z = rawValues.readFloat();
				
				dstPos.x = pos.x * matRaw[0] + pos.y * matRaw[4] + pos.z * matRaw[8];
				dstPos.y = pos.x * matRaw[1] + pos.y * matRaw[5] + pos.z * matRaw[9];
				dstPos.z = pos.x * matRaw[2] + pos.y * matRaw[6] + pos.z * matRaw[10];
				
				if(!isNormal)
				{
					dstPos.x += matRaw[12];
					dstPos.y += matRaw[13];
					dstPos.z += matRaw[14];
				}
				else
					dstPos.normalize();
				
				valuesOut.writeFloat( dstPos.x );
				valuesOut.writeFloat( dstPos.y );
				valuesOut.writeFloat( dstPos.z );
				
				for(var j:int=1; j<=floatStride-3; ++j)
					valuesOut.writeFloat( rawValues.readFloat() );
			}
			
			valuesOut.position = 0;
			outValues.position = 0;
			
			if(rawValues == outValues)
				outValues.writeBytes(valuesOut);
		}
		
		public static function leftMulTransformVectors(transform:Matrix3D, rawNumber:Vector.<Number>, stride:int, isNormal:Boolean = false):Vector.<Number>
		{
			var matRaw:Vector.<Number> = transform.rawData;
			var vecOut:Vector.<Number> = new Vector.<Number>;
			var len:int = rawNumber.length;
			var pos:Vector3D = new Vector3D;
			var dstPos:Vector3D = new Vector3D;
			for(var i:int=0; i<len; i+=stride)
			{
				pos.x = rawNumber[i];
				pos.y = rawNumber[i+1];
				pos.z = rawNumber[i+2];
				
				dstPos.x = pos.x * matRaw[0] + pos.y * matRaw[4] + pos.z * matRaw[8];
				dstPos.y = pos.x * matRaw[1] + pos.y * matRaw[5] + pos.z * matRaw[9];
				dstPos.z = pos.x * matRaw[2] + pos.y * matRaw[6] + pos.z * matRaw[10];
				
				if(!isNormal)
				{
					dstPos.x += matRaw[12];
					dstPos.y += matRaw[13];
					dstPos.z += matRaw[14];
				}
				
				vecOut.push( dstPos.x, dstPos.y, dstPos.z );
				for(var j:int=1; j<=stride-3; ++j)
					vecOut.push( rawNumber[i+2+j] );
			}
			
			return vecOut;
		}
		
		public function CMath()
		{
		}
	}
}