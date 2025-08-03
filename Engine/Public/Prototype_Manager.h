#pragma once
//#include "Base.h"

#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_MapToolTerrain.h"
#include "VIBuffer_Cube.h"
#include "SkyBoxCube.h"
#include "VIBuffer_Point_Instancing.h"
#include "Rectangle_Cube.h"
#include "VIBuffer_VerRect.h"
#include "TrailBuffer.h"


#include "Model.h"
#include "Navigation.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

// �� Prototype_Manager���� ������ ��ü����, Object_Manager�� ��Ƶΰ� �۾��ϰڴ�.
BEGIN(Engine)
class CPrototype_Manager final : public CBase
{
private:
    CPrototype_Manager();
    virtual ~CPrototype_Manager() = default;
    
public:
    HRESULT Initialize(_uint _iNumLevels);
    HRESULT Add_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag, CBase* _pPrototype);

    // ���� ��ü Ÿ������ (gameobject / component ���� ) ������, ã�� ������ü�� Ŭ���� CBase �θ�Ÿ�� Ŭ���� �������� return �Ѵ�.->GameObject�� ����ȯ ���� ����.
    // HRESULT Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strProtoTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg = nullptr);
    CBase* Clone_Prototype(PROTOTYPE _eType, _uint _iLevelIndex, const _wstring& _strPrototypeTag, void* _pArg);
    void Clear(_uint _iLevelIndex);


private:
    _uint       m_iNumLevels = {};

    map<const _wstring, CBase*>* m_mapPrototype = { nullptr };
    typedef map<const _wstring, CBase*> PROTOTYPES;

private:
    CBase* Find_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag);

public:
    static CPrototype_Manager* Create(_uint _iNumLevels);
    virtual void Free() override;

};

END