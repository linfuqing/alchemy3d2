package alchemy.effect
{
	import alchemy.core.CPointer;

	public class CAbstractEffectInfo extends CPointer
	{
		public function CAbstractEffectInfo(pointer:uint = 0)
		{
			_pointer = pointer;
		}
	}
}