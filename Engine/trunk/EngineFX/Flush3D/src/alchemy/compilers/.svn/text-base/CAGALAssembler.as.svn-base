package alchemy.compilers
{
	import alchemy.core.CLibrary;
	import alchemy.core.CObject;
	import alchemy.data.rendermethod.CParameterInfoData;
	import alchemy.data.rendersystem.CProgramData;
	import alchemy.resource.basic.CProgram3D;
	import alchemy.tools.CDebug;
	
	import com.adobe.utils.AGALMiniAssembler;
	
	import flash.display3D.Context3DProgramType;
	import flash.utils.ByteArray;

	public class CAGALAssembler extends CAbstractCompiler
	{
		public function set verbose(value:Boolean):void
		{
			_verbose = value;
		}
		
		public function get verbose():Boolean
		{
			return _verbose;
		}
		
		public function CAGALAssembler()
		{	
			super();
			
			init();
		}
		
		override protected function initPointers():Array
		{
			return CLibrary.method.CreateAGALAssembler();
		}
		
		override protected function compile(source:String, type:int):ByteArray
		{
			var programType:String = null;
			
			switch(type)
			{
				case CParameterInfoData.TYPE_VERTEX_SHADER:
					programType = Context3DProgramType.VERTEX;
					break;
				case CParameterInfoData.TYPE_FRAGMENT_SHADER:
					programType = Context3DProgramType.FRAGMENT;
					break;
				
				default:
					CDebug.warning("Invalid type");
					
					return null;
			}
			
			return _complier.assemble(programType, source, _verbose);
		}
		
		private function init():void
		{
			_complier = new AGALMiniAssembler(false);

			_verbose = false;
		}
		
		private var _complier:AGALMiniAssembler;
		
		private var _verbose:Boolean;
	}
}