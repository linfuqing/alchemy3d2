package alchemy.data.animation
{
	import alchemy.data.CSafeData;
	
	public class CAnimationSetData extends CSafeData
	{
		public var name:String;
		
		public function CAnimationSetData()
		{
			super();
			
			_animationTrackerDatas = new Vector.<CAnimationTrackData>;
		}
		
		public function get animationTrackerDatas():Vector.<CAnimationTrackData>
		{
			return _animationTrackerDatas;
		}
		
		protected var _animationTrackerDatas:Vector.<CAnimationTrackData>;
	}
}