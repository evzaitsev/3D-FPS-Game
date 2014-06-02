#ifndef VERTEX_H
#define VERTEX_H

namespace Vertex
{
	// Basic 32-byte vertex structure.
	struct Basic32
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct PosTex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};

	struct PosNormalTexTan
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT3 TangentU;
	};

	struct Terrain
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
		XMFLOAT2 BoundsY;
	};

	struct PosNormalTexTanSkinned
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 TangentU;
		XMFLOAT3 Weights;
		BYTE BoneIndices[4];
	};
}

class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC Terrain[3];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];
	static const D3D11_INPUT_ELEMENT_DESC InstancedBasic32[7];
	static const D3D11_INPUT_ELEMENT_DESC InstancedPosNormalTexTan[8];
	static const D3D11_INPUT_ELEMENT_DESC PosTex[2];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTanSkinned[6];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Pos;
	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* Terrain;
	static ID3D11InputLayout* PosNormalTexTan;
	static ID3D11InputLayout* InstancedBasic32;
	static ID3D11InputLayout* InstancedPosNormalTexTan;
	static ID3D11InputLayout* PosNormalTexTanSkinned;
	static ID3D11InputLayout* PosTex;
};

#endif // VERTEX_H