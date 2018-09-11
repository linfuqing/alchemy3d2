package alchemy.external.parser
{
	import alchemy.data.CSceneData;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.ISceneContainer;

	public interface ISceneDataParser
	{
		function onParse( datas:CSceneData ):Boolean;
		
		function onAddToScene( sceneContainer:ISceneContainer, effectHandle:uint = 0, renderMethod:CRenderMethod = null ):void;
	}
}