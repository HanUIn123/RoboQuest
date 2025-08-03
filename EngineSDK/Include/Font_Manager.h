#pragma once
#include "Base.h"


BEGIN(Engine)

class CFont_Manager final : public CBase
{
public:
    struct DamageText
    {
        _wstring text;          // ����� �ؽ�Ʈ (������ ��)
        _float3 position;       // �ؽ�Ʈ�� ��µ� ��ġ
        _float timeRemaining;   // �ؽ�Ʈ�� ȭ�鿡 ǥ�õ� �ð�
    };

public:

    _float2 Convert_WorldToScreen(const _float3& worldPosition);
private:
    CFont_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CFont_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath);
    HRESULT Render(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f), const _float _fScale = 1.f);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    map<const _wstring, class CCustomFont*>				m_mapFonts;




private:
    class CCustomFont* Find_Font(const _wstring& _strFontTag);

    vector<DamageText>      m_DamageTexts;


    class CGameInstance*              m_pGameInstance = { nullptr };
public:
    static CFont_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free() override;

};

END