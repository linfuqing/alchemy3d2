package alchemy.animation
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.data.animation.CBoneData;
	import alchemy.tools.CDebug;
	
	import flash.geom.Matrix3D;

	public class CSkinInfo extends CPointer
	{
		public function CSkinInfo( sourcePointer:uint = 0 )
		{
			if(sourcePointer)
				_pointer = sourcePointer;
			else
			{
				_pointer= CLibrary.method.CreateSkinInfo();
				CDebug.assert(_pointer != 0, "CSkinInfo::CreateSkinInfo() -- error!");
			}
		}
		
		public function set bonesData(val:Vector.<CBoneData>):void
		{
			CDebug.assert(val != null, "boneData can't be null!");
			
			var boneAryPos:uint = CLibrary.method.CreateSkinInfoData(val.length);
			
			var len:int = val.length;
			var boneInfoPtrs:Array;
			var boneData:CBoneData;
			
			var influence:uint;
			for(var i:int=0; i<len; ++i)
			{
				boneData = val[i];
				influence = boneData.weights.length;
				
				boneInfoPtrs = CLibrary.method.CreateBoneData( boneAryPos + i*CBoneData.BONE_DATA_OFFSET, influence , boneData.name.length+1  );
				
				CLibrary.memory.position = boneInfoPtrs[0];
				CLibrary.memory.writeUTFBytes( boneData.name );
				CLibrary.memory.writeByte(0);
				
				CLibrary.memory.position = boneInfoPtrs[1];
				for(var j:int=0; j<influence; ++j)
					CLibrary.memory.writeFloat( boneData.weights[j] );
				
				CLibrary.memory.position = boneInfoPtrs[2];
				for(j=0; j<influence; ++j)
					CLibrary.memory.writeUnsignedInt( boneData.indices[j] );
				
				CLibrary.memory.position = boneInfoPtrs[3];
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[0] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[4] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[8] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[12] );
				
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[1] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[5] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[9] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[13] );
				
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[2] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[6] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[10] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[14] );
				
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[3] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[7] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[11] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[15] );
			}
			
			CLibrary.method.SetBonesDataToSkinInfo(_pointer,boneAryPos,val.length);
			
			for(i=0; i<len; ++i)
				CLibrary.method.DestroyBoneData( boneAryPos + i*CBoneData.BONE_DATA_OFFSET );
			
			CLibrary.method.DestroySkinInfoData(boneAryPos);
		}
		
		override public function destroy():void
		{
			CLibrary.method.DestroySkinInfo(_pointer);
		}
	}
}