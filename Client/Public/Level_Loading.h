#pragma once

#include "Client_Defines.h"
#include "Level.h"

// 1. ���� ���� �ʿ�� �ϴ� �ڿ��� �غ��ؾ� �Ѵ�.
// 2. �ε�ȭ�� ��ü�� �����Ѵٴ� ���� ������� �Ѵ�. ( CPU ���� ó���� �ϴµ�, �̴� �����带 �̿��ؼ� ó�� ) 

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
    CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLevel_Loading() = default;

public:
    virtual HRESULT Initialize(LEVELID _eNextLevelID);
    virtual void    Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    LEVELID                 m_eNextLevelID = { LEVEL_END };
    class CLoader*          m_pLoader = { nullptr };

private:
    HRESULT Ready_Layer_StartBackGround(const _tchar* _pLayerTags);
    //HRESULT Ready_Layer_MousePosition(const _tchar* _pLayerTags);
    HRESULT Loading_StartBackGround();


    _tchar					            m_szLoadingText[MAX_PATH] = {};
public:
    // �ٸ� levelŬ������� �ٸ��� Create ��ü����, ������ �ε� ����, ��� ���������� � ��ü�� �ʿ��ϴ� ��� ������ �ʿ��ϹǷ�,
    // �߰������� ���ڿ� levelID�� �޾��ش�. (� ���� �ڿ������� ���� �ε����� �ǹ�)
    // ���� Create�� ���� �����Ǿ, �� Ŭ������ Initialize�� Level �θ�Ŭ������ Initialize �Լ��� �������̵��� �����Ѵ�. 
    static CLevel_Loading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID);
    virtual void Free() override;

};

END