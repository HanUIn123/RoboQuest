#include "../Public/Base.h"

CBase::CBase()
{
}

_uint CBase::AddRef()
{
    return ++m_iRefCount;
}

_uint CBase::Release()
{
    if (0 == m_iRefCount)
    {
        Free();

        delete this;

        return 0;
    }
    else
        return m_iRefCount--;
}

void CBase::Free()
{
}
