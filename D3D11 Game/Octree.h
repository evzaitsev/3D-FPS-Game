//***************************************************************************************
// Octree.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple octree for doing ray/triangle intersection queries.
//***************************************************************************************

#ifndef OCTREE_H
#define OCTREE_H

#include "d3dUtil.h"
#include "XnaCollision.h"

struct OctreeNode;

class Octree
{
public:
	struct Object
	{

		//this would be output by the function
		OctreeNode** node;


		//to be computed by user 
		XNA::AxisAlignedBox Box;

	};

public:
	Octree();
	~Octree();

	void Build(const std::vector<XMFLOAT3>& vertices, const std::vector<UINT>& indices);
	bool RayOctreeIntersect(FXMVECTOR rayPos, FXMVECTOR rayDir);

	void AddObject(Object& inout);
	void Update(CXMMATRIX W);
	
private:
	XNA::AxisAlignedBox BuildAABB();
	void BuildOctree(OctreeNode* parent, const std::vector<UINT>& indices);
	bool RayOctreeIntersect(OctreeNode* parent, FXMVECTOR rayPos, FXMVECTOR rayDir);
	OctreeNode** AddObject(OctreeNode* parent, XNA::AxisAlignedBox* box);
	void Update(CXMMATRIX W, OctreeNode* parent);
	void SetVisible(OctreeNode* parent, bool visible);
private:
	OctreeNode* mRoot;
 
	std::vector<XMFLOAT3> mVertices;
};

struct OctreeNode
{
	#pragma region Properties
	XNA::AxisAlignedBox Bounds;

	// This will be empty except for leaf nodes.
	std::vector<UINT> Indices;

	bool Visible;

	OctreeNode* Children[8];

	bool IsLeaf;
	#pragma endregion

	OctreeNode()
	{
		for(int i = 0; i < 8; ++i)
			Children[i] = nullptr;

		Bounds.Center  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Bounds.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);

		IsLeaf = false;
		Visible = false;
	}

	~OctreeNode()
	{
		for(int i = 0; i < 8; ++i)
			SafeDelete(Children[i]);
	}

	///<summary>
	/// Subdivides the bounding box of this node into eight subboxes (vMin[i], vMax[i]) for i = 0:7.
	///</summary>
	void Subdivide(XNA::AxisAlignedBox box[8])
	{
		XMFLOAT3 halfExtent(
			0.5f*Bounds.Extents.x,
			0.5f*Bounds.Extents.y,
			0.5f*Bounds.Extents.z);

		// "Top" four quadrants.
		box[0].Center  = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[0].Extents = halfExtent;

		box[1].Center  = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[1].Extents = halfExtent;

		box[2].Center  = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[2].Extents = halfExtent;

		box[3].Center  = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[3].Extents = halfExtent;

		// "Bottom" four quadrants.
		box[4].Center  = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[4].Extents = halfExtent;

		box[5].Center  = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[5].Extents = halfExtent;

		box[6].Center  = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[6].Extents = halfExtent;

		box[7].Center  = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[7].Extents = halfExtent;
	}
};

#endif // OCTREE_H