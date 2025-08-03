// 커맨드 명령어	옵션		원본 파일이 있는 위치		사본 파일을 저장할 위치

xcopy		/y/s		.\Engine\Public\*.*		.\EngineSDK\Include\

xcopy		/y		.\Engine\Bin\Engine.dll	.\Client\Bin\
xcopy		/y		.\Engine\Bin\Engine.lib	.\EngineSDK\Library\	

xcopy		/y		.\Engine\ThirdPartyLib\*.lib .\EngineSDK\Library\

xcopy			/y				.\Engine\Bin\ShaderFiles\*.*			.\EngineSDK\Hlsl\