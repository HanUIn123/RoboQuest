#pragma once
#include "Base.h"

BEGIN(Engine)
class CPipeLine final : public CBase
{
public:
    enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
    CPipeLine();
    virtual ~CPipeLine() = default;

public:
    _float4x4       Get_Transform_Float4x4(D3DTRANSFORMSTATE _eState) const
    {
        return m_TransformationMatrixArray[_eState];
    }
    _matrix         Get_Transform_Matrix(D3DTRANSFORMSTATE _eState) const
    {
        return XMLoadFloat4x4(&m_TransformationMatrixArray[_eState]);
    }

    _float4x4       Get_Transform_Float4x4_Inverse(D3DTRANSFORMSTATE _eState) const
    {
        return m_TransformationInverseMatrixArray[_eState];
    }

    _matrix         Get_Transform_Matrix_Inverse(D3DTRANSFORMSTATE _eState) const
    {
        return XMLoadFloat4x4(&m_TransformationInverseMatrixArray[_eState]);
    }

    _float4     Get_CameraPos() const
    {
        return m_vCameraPos;
    }

public:
    void Set_Transform(D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix) {
        XMStoreFloat4x4(&m_TransformationMatrixArray[_eState], _TransformMatrix);
    }

public:
    HRESULT         Initialize();
    void            Update();

private:
    _float4x4   m_TransformationMatrixArray[D3DTS_END] = {};
    _float4x4   m_TransformationInverseMatrixArray[D3DTS_END] = {};
    _float4     m_vCameraPos = {};

public:
    static CPipeLine* Create();
    virtual void Free() override;

};

END