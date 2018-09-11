package alchemy.scene
{
	import alchemy.core.CLibrary;
	import alchemy.data.grasssystem.CGrassMapData;
	import alchemy.data.grasssystem.CGrassMapInfoData;
	import alchemy.data.grasssystem.CGrassTerrainData;
	import alchemy.manager.CMeshManager;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.tools.CDebug;

	public class CGrassSystem extends CSceneNode
	{
		public function CGrassSystem(grassMesh:uint, subset:uint, grassMapInfo:CGrassMapInfoData, terrain:Vector.<CGrassTerrainData>) 
		{
			CDebug.assert(grassMesh != 0, "need grass mesh info.");
			CDebug.assert(grassMapInfo != null, "need grass map info.");
			
			_grassMesh    = grassMesh;
			_subset       = subset;
			_grassMapInfo = grassMapInfo;
			_terrain      = terrain;
			
			super(0, 0, 0);
		}
		
		public function set material( materialHandle:uint ):void
		{
			if(_pointer && materialHandle)
				CLibrary.method.SetMaterialToGrassSystem(_pointer, materialHandle);
		}
		
		public function set effect(handle:uint):void
		{
			if(_pointer && handle)
				CLibrary.method.SetEffectToGrassSystem(_pointer, handle);
		}
		
		public function setRenderMethod(renderMethod:CRenderMethod):void
		{
			CLibrary.method.SetRenderMethodToGrassSystem(_pointer,renderMethod.pointer);
		}
		
		override protected function getSceneNodePointers():Array
		{
			CMeshManager.instance;
			
			var grassMapLength:uint = _grassMapInfo.grassMaps.length;
			var grassMapDataSize:uint = _grassMapInfo.width * _grassMapInfo.height * CLibrary.INT_SIZE;
			var bufferSize:uint = CGrassMapInfoData.SIZE + grassMapLength * (CGrassMapData.SIZE + grassMapDataSize);
			var i:uint;
			var grassMap:CGrassMapData;
			for(i = 0; i < grassMapLength; ++ i)
			{
				grassMap = _grassMapInfo.grassMaps[i];
				bufferSize += grassMap.channelMask.length * CLibrary.INT_SIZE;
				bufferSize += grassMap.channelScale.length * CLibrary.FLOAT_SIZE;
			}
			
			if(_terrain)
			{
				var terrainLength:uint = _terrain.length;
				bufferSize += terrainLength * CGrassTerrainData.SIZE;
				
				var matrixSize:uint = 16 * CLibrary.FLOAT_SIZE;
				for(i = 0; i < terrainLength; ++ i)
				{
					if(_terrain[i].transform)
						bufferSize += matrixSize;
				}
			}
			
			var bufferPointer:uint = CLibrary.method.CreateBuffer(bufferSize);
			var grassMapInfoPointer:uint = bufferPointer;
			CLibrary.memory.position = grassMapInfoPointer;
			CLibrary.memory.writeUnsignedInt(_grassMapInfo.width);
			CLibrary.memory.writeUnsignedInt(_grassMapInfo.height);
			CLibrary.memory.writeFloat(_grassMapInfo.scale);
			var grassMapsPointer:uint = grassMapInfoPointer + CGrassMapInfoData.SIZE;
			CLibrary.memory.writeUnsignedInt(grassMapsPointer);
			CLibrary.memory.writeUnsignedInt(grassMapLength);
			
			var dataPointer:uint = grassMapsPointer + CGrassMapData.SIZE * grassMapLength;
			var offsetPointer:uint = dataPointer;
			CLibrary.memory.position = grassMapsPointer;
			for(i = 0; i < grassMapLength; ++ i)
			{
				grassMap = _grassMapInfo.grassMaps[i];
				CLibrary.memory.writeUnsignedInt(offsetPointer);
				offsetPointer += grassMapDataSize;
				CLibrary.memory.writeUnsignedInt(grassMap.colorMask);
				CLibrary.memory.writeUnsignedInt(4);
				CLibrary.memory.writeUnsignedInt(grassMap.mapUsage);
				CLibrary.memory.writeUnsignedInt(offsetPointer);
				offsetPointer += grassMap.channelMask.length * CLibrary.INT_SIZE;
				CLibrary.memory.writeUnsignedInt(offsetPointer);
				offsetPointer += grassMap.channelScale.length * CLibrary.FLOAT_SIZE;
				CLibrary.memory.writeUnsignedInt(_grassMapInfo.width * 4);
			}
			
			var j:uint;
			var length:uint;
			CLibrary.memory.position = dataPointer;
			for(i = 0; i < grassMapLength; ++ i)
			{
				grassMap = _grassMapInfo.grassMaps[i];
				CLibrary.memory.writeBytes(grassMap.colorData, 0, grassMapDataSize);
				
				length = grassMap.channelMask.length;
				for(j = 0; j < length; ++ j)
					CLibrary.memory.writeUnsignedInt(grassMap.channelMask[j]);
				
				length = grassMap.channelScale.length;
				for(j = 0; j < length; ++ j)
					CLibrary.memory.writeFloat(grassMap.channelScale[j]);
			}
			
			var terrainPointer:uint = 0;
			if(_terrain)
			{
				var terrain:CGrassTerrainData;
				terrainPointer = offsetPointer;
				
				var transformPointer:uint = terrainPointer + CGrassTerrainData.SIZE * terrainLength;
				offsetPointer = transformPointer;
				
				CLibrary.memory.position = terrainPointer;
				for(i = 0; i < terrainLength; ++ i)
				{
					terrain = _terrain[i];
					CLibrary.memory.writeShort(terrain.mesh);
					CLibrary.memory.writeShort(0);
					CLibrary.memory.writeFloat(terrain.up.x);
					CLibrary.memory.writeFloat(terrain.up.y);
					CLibrary.memory.writeFloat(terrain.up.z);
					if(terrain.transform)
					{
						CLibrary.memory.writeUnsignedInt(offsetPointer);
						offsetPointer += matrixSize;
					}
					else
						CLibrary.memory.writeUnsignedInt(0);
					
					//CLibrary.memory.writeShort(0);
				}
				
				var rawData:Vector.<Number>;
				
				CLibrary.memory.position = transformPointer;
				for(i = 0; i < terrainLength; ++ i)
				{
					terrain = _terrain[i];
					if(terrain.transform)
					{
						rawData = terrain.transform.rawData;
						CLibrary.memory.writeFloat(rawData[0]);
						CLibrary.memory.writeFloat(rawData[4]);
						CLibrary.memory.writeFloat(rawData[8]);
						CLibrary.memory.writeFloat(rawData[12]);
						
						CLibrary.memory.writeFloat(rawData[1]);
						CLibrary.memory.writeFloat(rawData[5]);
						CLibrary.memory.writeFloat(rawData[9]);
						CLibrary.memory.writeFloat(rawData[13]);
						
						CLibrary.memory.writeFloat(rawData[2]);
						CLibrary.memory.writeFloat(rawData[6]);
						CLibrary.memory.writeFloat(rawData[10]);
						CLibrary.memory.writeFloat(rawData[14]);
						
						CLibrary.memory.writeFloat(rawData[3]);
						CLibrary.memory.writeFloat(rawData[7]);
						CLibrary.memory.writeFloat(rawData[11]);
						CLibrary.memory.writeFloat(rawData[15]);
					}
				}
			}
			
			var pointers:Array = CLibrary.method.CreateGrassSystem(_grassMesh, _subset, grassMapInfoPointer, terrainPointer, terrainLength);
			
			CLibrary.method.DestroyBuffer(bufferPointer);
			
			return pointers;
		}
		
		private var _grassMesh:uint;
		private var _subset:uint;
		private var _grassMapInfo:CGrassMapInfoData;
		private var _terrain:Vector.<CGrassTerrainData>;
	}
}