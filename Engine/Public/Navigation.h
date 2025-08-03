#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CNavigation final : public CComponent
{
public:
    struct NAVIGATION_DESC
    {
        _int            iCurrentCellIndex = { - 1};
    };

private:
    CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CNavigation(const CNavigation& _Prototype);
    virtual ~CNavigation() = default;

public:
    bool Get_FirstPick() const
    {
        return m_bFirstPick;
    }

    void Set_FirstPick(bool bFirstPick)
    {
        m_bFirstPick = bFirstPick;
    }

public:
    //vector<class CCell*> Get_Cells() const{return m_vecCells;}
    const vector<class CCell*>& Get_Cells() const { return m_vecCells; }

public:
    virtual HRESULT                 Initialize_Prototype(const _tchar* _pNavigationDataFile);
    HRESULT                         Initialize_Prototype_CreateOnly();
    virtual HRESULT                 Initialize(void* _pArg) override;

public:
    // �� ���� ��Ŀ�, ���ڷ� �޾ƿ� _WorldMatrix �� �����ض� ��� �ǹ��� �Լ�.
    void SetUp_WorldMatrix(_fmatrix _WorldMatrix)
    {
        XMStoreFloat4x4(&m_WorldMatrix, _WorldMatrix);
        XMStoreFloat4x4(&m_WorldMatrixInverse, XMMatrixInverse(nullptr, _WorldMatrix));
    }

    void Set_CurrentNaviIndex(_vector _vWorldPos);

public:
    _bool                           bIsMove(_fvector _vWorldPos);
    _float                          Compute_Height(_fvector _vWorldPos);
    _vector                         Setting_SlidingMove(_fvector _vWorldPos);

    void                            Setting_ShrinkCell(_float _fShrinkRatio);
    _bool                           bIsIn_ShrinkedCell(_fvector _vWorldPos);
    _bool                           bIsOn_Line(_fvector _vWorldPos);

    _bool                           m_bFirstPick = { true };

#ifdef _DEBUG
public:
    HRESULT Render();
#endif

private:
    // �׺���̼��� �̿��ϴ� ��ü�� � ���� �ε����� ���� ���̴�~~ ��� ������ ��밡��. 
    _int                           m_iCurrentCellIndex = { -1 };
    //���� �����ؼ� ��ü�� �������� �ȳ������� �Ǵ��ϱ� ���� �� ���� �����ϴ� ��찡 �����Ͱ��Ƽ�.
    vector<class CCell*>            m_vecCells;

private:
    static _float4x4                m_WorldMatrix;
    static _float4x4                m_WorldMatrixInverse;
    int                             m_iNeighborIndex = {};


    _uint                           m_iSlidingLineIndex = -1;
    _bool                           m_bSliding = false;

#ifdef _DEBUG
private:
    class CShader* m_pShader = { nullptr };
#endif

private:
    HRESULT     SetUp_NeighborCells();


public:
    static CNavigation* CreateOnly(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    static CNavigation* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pNavigationDataFile);

    HRESULT Create_Cell(const _float3 _vPoints[3]);
    HRESULT Delete_Cell(const _float3 _vPoints[3]);
    virtual CComponent* Clone(void* _pArg)  override;
    virtual void Free() override;
};

END