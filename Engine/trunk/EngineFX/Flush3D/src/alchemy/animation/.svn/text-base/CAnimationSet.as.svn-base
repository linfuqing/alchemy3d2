package alchemy.animation
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.data.animation.CAnimationTrackData;

	public class CAnimationSet extends CPointer
	{	
		public function CAnimationSet( sourcePointer:uint = 0 )
		{
			if(!sourcePointer)
				_pointer = CLibrary.method.CreateAniamtionSet();
			else
				_pointer = sourcePointer;
		}
		
		public function getPlayTime():Number
		{
			return _pointer ? CLibrary.method.GetPlayTimeFromAnimationSet(_pointer) : 0.0;
		}
		
		public function set animationTracks(val:Vector.<CAnimationTrackData>):void
		{
			var len:int = val.length;
			var trackPointer:uint = CLibrary.method.CreateAnimationSetData(len);
			var animTrack:CAnimationTrackData;
			var trackPtrs:Array;
			var frameLen:int;
			var i:int;
			var j:int;
			var c:int;
			
			for(i=0; i<len; ++i)
			{
				animTrack = val[i];
				frameLen = animTrack.times.length;
				trackPtrs = CLibrary.method.CreateAnimationTracker(trackPointer+i*CAnimationTrackData.DATA_OFFSET,animTrack.name.length+1,frameLen);
				
				CLibrary.memory.position = trackPtrs[0];
				CLibrary.memory.writeUTFBytes( animTrack.name );
				CLibrary.memory.writeByte(0);
				
				CLibrary.memory.position = trackPtrs[1];
				for(j=0; j<frameLen; ++j)
					CLibrary.memory.writeFloat( animTrack.times[j] );
				
				CLibrary.memory.position = trackPtrs[2];
				for(j=0; j<frameLen; ++j)
				{
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[0] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[4] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[8] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[12] );
					
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[1] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[5] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[9] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[13] );
					
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[2] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[6] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[10] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[14] );
					
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[3] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[7] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[11] );
					CLibrary.memory.writeFloat( animTrack.transformMatrixes[j].rawData[15] );
				}
			}
			
			CLibrary.method.SetTrackersToAnimationSet(_pointer,trackPointer,len);
			
			for(i=0; i<len; ++i)
				CLibrary.method.DestroyAnimTrack( trackPointer + i*CAnimationTrackData.DATA_OFFSET );
			
			CLibrary.method.DestroyAnimSetData(trackPointer);
		}
		
		override public function destroy():void
		{
			CLibrary.method.DestroyAnimSet(_pointer);
		}
		
//		public function get trackCount():int
//		{
//			if(_animationTracks == null)
//				return 0;
//			
//			return _animationTracks.length;
//		}
//		
//		public function get totalTime():Number
//		{
//			if(_animationTracks == null)
//				return 0;
//			
//			return _animationTracks[0].times[  _animationTracks[0].times.length-1 ];
//		}
		
		//protected var _animationTracks:Vector.<CAnimationTrackData>;
	}
}