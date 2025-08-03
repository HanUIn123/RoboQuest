#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
    typedef struct
    {
        HINSTANCE       hInstance;
        HWND            hWnd;
        unsigned int    iNumLevels;
        bool            isWindowed;
        unsigned int    iViewportWidth;
        unsigned int    iViewportHeight;

    }ENGINE_DESC;

    struct LIGHT_DESC
    {
        enum LIGHT_TYPE { LT_DIRECTIONAL, LT_POINT, LT_SPOTLIGHT, LT_END };
        LIGHT_TYPE eState;

        XMFLOAT4    vDirection;
        XMFLOAT4    vPosition;
        float       fRange;



        XMFLOAT4    vDiffuse;
        XMFLOAT4    vAmbient;
        XMFLOAT4    vSpecular;
    };

    struct MATERIAL_DESC
    {
        XMFLOAT4    vDiffuse;
        XMFLOAT4    vAmbient;
        XMFLOAT4    vSpecular;
        float       fPower;
        XMFLOAT4    vEmissive;
    };

    struct ENGINE_DLL VTXPOSTEX
    {
        XMFLOAT3                                    vPosition;
        XMFLOAT2                                    vTexCoord;

        // 구조체 안에 선언을 하되, 구조체 할당할 때, 같이 할당 되는게 아니라, 
        // 컴파일 단계에서 미리 할당 한 번 하는 형태로, static 으로 선언한다.
        // 이 정점 구조체는 멤버 변수를 2개 가지고 있다! 
        const static unsigned int                   iNumElements = 2;

        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };

    struct ENGINE_DLL VTXPOS
    {
        XMFLOAT3                                    vPosition;

        const static unsigned int                   iNumElements = 1;
        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };

    struct ENGINE_DLL VTXCUBE
    {
        XMFLOAT3		vPosition;
        XMFLOAT3		vTexcoord;

        const static unsigned int					iNumElements = 2;
        const static D3D11_INPUT_ELEMENT_DESC		tagElements[iNumElements];
    };

    struct ENGINE_DLL VTXPOINT
    {
        XMFLOAT3        vPosition;
        float           fPointSize;

        const static unsigned int                   iNumElements = 2;
        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };


    struct ENGINE_DLL VTX_PARTICLE_INSTANCE
    {
        // 정점을 가장한 행렬정보를 심어주자. 
        // 정점 하나를 사각형 하나 제어하기 위한 행렬로 사용할 것.
        XMFLOAT4            vRight;
        XMFLOAT4            vUp;
        XMFLOAT4            vLook;
        XMFLOAT4            vTranslation;
        XMFLOAT2			vLifeTime;
    };


    struct ENGINE_DLL VTXNORTEX
    {
        XMFLOAT3                                    vPosition;
        XMFLOAT3                                    vNormal;
        XMFLOAT2                                    vTexCoord;
        XMFLOAT3                                    vTexIndex;

        const static unsigned int                   iNumElements = 4;
        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };



    struct ENGINE_DLL VTXTERRAINTEX
    {
        XMFLOAT3                                    vPosition;
        XMFLOAT2                                    vTexCoord;
        const static unsigned int                   iNumElements = 2;
        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };

    struct ENGINE_DLL VTXMESH
    {
        XMFLOAT3                                    vPosition;
        XMFLOAT3                                    vNormal;
        XMFLOAT2                                    vTexCoord;
        XMFLOAT3                                    vTangent;

        const static unsigned int                   iNumElements = 4;
        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };

    struct ENGINE_DLL VTXANIMMESH
    {
        XMFLOAT3                                    vPosition;
        XMFLOAT3                                    vNormal;
        XMFLOAT2                                    vTexCoord;
        XMFLOAT3                                    vTangent;

        XMUINT4			                            vBlendIndex;
        XMFLOAT4		                            vBlendWeight;

        const static unsigned int                   iNumElements = 6;
        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };

    struct KEYFRAME
    {
        XMFLOAT3            vScale;
        XMFLOAT4            vRotation;
        XMFLOAT3            vTranslation;

        // 어떤 상태의 시간?
        float               fTrackPosition;

    };


    typedef struct tagIndex16
    {
        _ushort  _0;
        _ushort  _1;
        _ushort  _2;

    }INDEX16;

    struct ENGINE_DLL VTX_POINT_INSTANCE
    {
        const static unsigned int                   iNumElements = 7;
        const static D3D11_INPUT_ELEMENT_DESC       tagElements[iNumElements];
    };


    // 사운드 채널
    enum CHANNELID
    {
        SOUND_EFFECT, SOUND_IMPORTANT, SOUND_BGM,
        //SOUND_INSTRUMENTS_HARP, SOUND_INSTRUMENTS_CELLO,
        //SOUND_INSTRUMENTS_FLUTE,
        //SOUND_INSTRUMENTS_OCARINA,
        //SOUND_INSTRUMENTS_DRUM,
        //SOUND_INSTRUMENTS_PIANO,
        //SOUND_PLAYER, SOUND_EAT, SOUND_FIRE, SOUND_PET, SOUND_DEAD,
        //SOUND_SHAMAN, SOUND_MUSHROOM, SOUND_SLIME, SOUND_HUNTER,
        //SOUND_PLANT,

        //SOUND_SPAWN_MALU, SOUND_SPAWN_AZEOS,
        //SOUND_MALUGAZ, SOUND_AZEOS,

        //SOUND_AZEOS_SHADOW,


        //SOUND_WALL_DOWN,
        //SOUND_ITEM_PICKUP,

        //SOUND_UI_INVENTORY,

     

        PLAYER_MOVE,
        PLAYER_SHOOT,
        PLAYER_JUMP,
        PLAYER_DASH,
        PLAYER_RELOAD,
        PLAYER_RIFLE_SHOOT,
        PLAYER_SNIPER_SHOOT,
        PLAYER_ZOOM_IN,
        PLAYER_ZOOM_OUT,
        BOSS_INTRO,
        BOSS_NORMAL_SHOOT,
        BOSS_MORTAL_SHOOT,
        BSSS_FIRETHROW,
        BOSS_BARRIER,

        MONSTER_SHOOT,
        MONSTER_MORTAL_SHOOT,
        MONSTER_DEAD,

        NPC_GLAD,

        MAXCHANNEL
    };
}


#endif // Engine_Struct_h__
