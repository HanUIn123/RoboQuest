#pragma once
#include "Client_Defines.h"
#include "Camera.h"

//BEGIN(Engine)

BEGIN(Client)
class CCamera_Free final : public CCamera
{
public:
    struct      CAMERA_FREE_DESC : public CAMERA_DESC
    {
        _float          fMouseSensity;
    };


private:
     CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext) ;
     CCamera_Free(const CCamera_Free& _Prototype);
     virtual ~CCamera_Free() = default;

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

    void                Toggle_Tab(_float _fTimeDelta);
    void                Switch_To_FPV(_float _fTimeDelta);

private:
    _float              m_fMouseSensity;
    _bool               m_bTabbed = { false };
    _bool               m_bPressed = { false };
    CTransform*         m_pTargetTransform = { nullptr };

private:
    _bool               m_bFPV = { false };

    // 클래스 멤버 변수 추가
    float m_fCameraPosX = 0.0f;
    float m_fCameraPosY = 0.0f;
    float m_fCameraPosZ = 0.0f;

public:
    static CCamera_Free* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;



};

END