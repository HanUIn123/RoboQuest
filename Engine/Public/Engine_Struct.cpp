#include "Engine_Define.h"
namespace Engine
{
    // Engine_struct.h ����, const static �ϰ�, �迭�� ������ �� ���⼭ �ʱ�ȭ �����ִ� ��.
    // static ���� ������ �Ϲ� ������ (int .. ��) �� ���� cpp ��ܿ� �ʱ�ȭ�� ����� ������, �׳� const �ٿ��� �ش� h���Ͽ��� �׳�
    // �ʱ�ȭ �ϰ� ���� �� �ִµ�, �迭�� �� �װ� �ȵǼ�, �̷��� �����������.

    // ó����, ��ȣ���� ������ �����µ�, �̰���, cpp �� ���ǵ� ������ Client �� �𸣱� ������, libȭ �ؼ� ����������Ѵ�.
    // ���� Sturct.h ���� Engine_DLL Ű���带 �ٿ��� " Client �� ������ ���̴� " ��� ����ϰ�, ���� cpp ���Ͽ�����
    // const static �迭 �ι� �ʱ�ȭ�� ����.
    const D3D11_INPUT_ELEMENT_DESC VTXPOS::tagElements[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0 ,0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    const D3D11_INPUT_ELEMENT_DESC VTXPOSTEX::tagElements[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0 ,0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    const D3D11_INPUT_ELEMENT_DESC VTXCUBE::tagElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
    };

    const D3D11_INPUT_ELEMENT_DESC VTXTERRAINTEX::tagElements[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0 ,0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    const D3D11_INPUT_ELEMENT_DESC VTXNORTEX::tagElements[] =
    {
        {"POSITION" ,0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    const D3D11_INPUT_ELEMENT_DESC VTXMESH::tagElements[] =
    {
        {"POSITION" ,0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    const D3D11_INPUT_ELEMENT_DESC VTXANIMMESH::tagElements[] =
    {
        {"POSITION" ,0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA , 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA , 0 }
    };

    const D3D11_INPUT_ELEMENT_DESC VTX_POINT_INSTANCE::tagElements[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },

    // WORLD ��� �ø�ƽ�� �ٿ��ָ鼭, ���� �ٷ� ���̴��� ���� �������� �Ѱ��ַ��� ��.
    { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA , 1 },
    { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA , 1 },
    { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA , 1 },
    { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA , 1 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA , 1 },
    };
}




