#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CVIBuffer_MapToolTerrain;
END

BEGIN(Client)
class CCamera_MapTool final : public CCamera
{
public:
    struct      CAMERA_MAPTOOL_DESC : public CAMERA_DESC
    {
        _float          fMouseSensity;
    };

public:
    CVIBuffer_MapToolTerrain* Get_MapToolTerrain() { return m_pMTTerrain_BufferCom; }


private:
    CCamera_MapTool (ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CCamera_MapTool (const CCamera_MapTool & _Prototype);
    virtual ~CCamera_MapTool () = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual void    Priority_Update(_float _fTimeDelta) override;
    virtual void    Late_Update(_float _fTimeDelta) override;
    virtual void    Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;


private:
    void                Fix_MouseCursor();
    void                Move_Mouse(_float _fTimeDelta);

    void                Move_Camera(_float _fTimeDelta);
    void                Toggle_Button(_float _fTimeDelta);
    void                Switch_To_FPV(_float _fTimeDelta);

    void                Click_Terrain(_float _fTimDelta);

public:
    void                Switch_SightView();


private:
    _float                      m_fMouseSensity;

    // 카메라 Tab키 업데이트 호출 여러번 방지용 bool 값
    _bool                       m_bTabbed = { false };
    _bool                       m_bPressed = { false };

    // 마우스 클릭 업데이트 호출 여러번 방지용 bool 값
    _bool                       m_bClicked = { false };
    _bool                       m_bIsClicked = { false };


    _bool                       m_bCameraFix = { false };

    CTransform*                 m_pTargetTransform = { nullptr };
    CVIBuffer_MapToolTerrain*   m_pMTTerrain_BufferCom = { nullptr };

public:
    static CCamera_MapTool * Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END