package alchemy.scene.particlesystem
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CParticleDataCode
	{
		public function CParticleDataCode()
		{
			var ptrs:Array = CLibrary.method.CreateParticleDataCode();
			CDebug.assert( ptrs != null, "CParticleDataCode::CreateParticleDataCode() - error");
			
			_codePtr = ptrs[0];
			_definerPtr = ptrs[1];
		}
		
		public function appendComputeInfo( computeInfo:CAbstractParticleComputeInfo ):void
		{
			CLibrary.method.AddParticleComputeInfoToDataCode(_definerPtr, computeInfo.interfacePointer );
		}
		
		public function appendVariableInfo( variableComputeInfo:CAbstractParticleVariableComputeInfo ):void
		{
			CLibrary.method.AddParticleVariableInfoToDataCode(_definerPtr, variableComputeInfo.interfacePointer );
		}
		
		public function buildController():CParticleController 
		{
			var ptrs:Array = CLibrary.method.BuildParticleController( _definerPtr );
			
			CDebug.assert( ptrs!=null,"CParticleDataCode::buildController() -- create error!" );
			
			return new CParticleController( ptrs[0], ptrs[1] );
		}
		
		private var _codePtr:uint;
		private var _definerPtr:uint;
	}
}