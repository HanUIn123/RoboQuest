#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
    CBase();
    virtual ~CBase() = default;

public:
    _uint AddRef();

    _uint Release();

private:
    _uint m_iRefCount = { 0 };

public:
    virtual void Free();

};

END