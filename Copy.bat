// Ŀ�ǵ� ��ɾ�	�ɼ�		���� ������ �ִ� ��ġ		�纻 ������ ������ ��ġ

xcopy		/y/s		.\Engine\Public\*.*		.\EngineSDK\Include\

xcopy		/y		.\Engine\Bin\Engine.dll	.\Client\Bin\
xcopy		/y		.\Engine\Bin\Engine.lib	.\EngineSDK\Library\	

xcopy		/y		.\Engine\ThirdPartyLib\*.lib .\EngineSDK\Library\

xcopy			/y				.\Engine\Bin\ShaderFiles\*.*			.\EngineSDK\Hlsl\