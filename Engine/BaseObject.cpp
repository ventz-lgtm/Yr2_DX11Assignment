/**
NIEE2211 - Computer Games Studio 2

Filename: BaseObject.cpp
Author: Daniel Lush
Date: 13/12/2018
*/

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "BaseObject.h"
#include "World.h"
#include "graphicsclass.h"
#include "HitResult.h"

// The base object class is a generic class which contains information which all objects can use for common purposes
// This reduces the amount of repeated code and allows for faster addition of many objects

VertexData BaseObject::VerticesFromBoundingBox(ObjectBoundingBox * pBoundingBox, bool shiftOrigin)
{
	XMFLOAT3 min = *pBoundingBox->pMins;
	XMFLOAT3 max = *pBoundingBox->pMaxs;
	XMFLOAT3 diff = XMFLOAT3(max.x - min.x, max.y - min.y, max.z - min.z);

	VertexData data = VertexData();
	data.numIndices = 8;
	data.numTriangles = 36;

	data.vertices[0] = XMFLOAT3(0, 0, 0);
	data.uv[0] = XMFLOAT2(0, 0);
	data.vertices[1] = XMFLOAT3(diff.x, 0, 0);
	data.uv[1] = XMFLOAT2(1, 0);
	data.vertices[2] = XMFLOAT3(diff.x, diff.y, 0);
	data.uv[2] = XMFLOAT2(1, 1);
	data.vertices[3] = XMFLOAT3(0, diff.y, 0);
	data.uv[3] = XMFLOAT2(0, 1);
	data.vertices[4] = XMFLOAT3(0, diff.y, diff.z);
	data.uv[4] = XMFLOAT2(0, 0);
	data.vertices[5] = XMFLOAT3(diff.x, diff.y, diff.z);
	data.uv[5] = XMFLOAT2(1, 0);
	data.vertices[6] = XMFLOAT3(diff.x, 0, diff.z);
	data.uv[6] = XMFLOAT2(1, 1);
	data.vertices[7] = XMFLOAT3(0, 0, diff.z);
	data.uv[7] = XMFLOAT2(1, 1);

	if (shiftOrigin) {
		for (int i = 0; i < data.numIndices; i++) {

			XMFLOAT3 hd = XMFLOAT3(min.x, min.y, min.z);
			data.vertices[i] = XMFLOAT3(data.vertices[i].x + hd.x, data.vertices[i].y + hd.y, data.vertices[i].z + hd.z);
		}
	}

	data.triangles[0] = 0;
	data.triangles[1] = 2;
	data.triangles[2] = 1;

	data.triangles[3] = 0;
	data.triangles[4] = 3;
	data.triangles[5] = 2;

	data.triangles[6] = 2;
	data.triangles[7] = 3;
	data.triangles[8] = 4;

	data.triangles[9] = 2;
	data.triangles[10] = 4;
	data.triangles[11] = 5;

	data.triangles[12] = 1;
	data.triangles[13] = 2;
	data.triangles[14] = 5;

	data.triangles[15] = 1;
	data.triangles[16] = 5;
	data.triangles[17] = 6;

	data.triangles[18] = 0;
	data.triangles[19] = 7;
	data.triangles[20] = 4;

	data.triangles[21] = 0;
	data.triangles[22] = 4;
	data.triangles[23] = 3;

	data.triangles[24] = 5;
	data.triangles[25] = 4;
	data.triangles[26] = 7;

	data.triangles[27] = 5;
	data.triangles[28] = 7;
	data.triangles[29] = 6;

	data.triangles[30] = 0;
	data.triangles[31] = 6;
	data.triangles[32] = 7;

	data.triangles[33] = 0;
	data.triangles[34] = 1;
	data.triangles[35] = 6;

	return data;
}

BaseObject::BaseObject(const char* Name, const char* ModelPath, WCHAR* MaterialPath, WCHAR* MaterialPath2)
{
	this->Name = Name;

	mDestroyed = false;
	pScale = new XMFLOAT3(1.f, 1.f, 1.f);
	pPosition = new XMFLOAT3(0.f, 0.f, 0.f);
	pVelocity = new XMFLOAT3(0.f, 0.f, 0.f);
	pAngle = new XMFLOAT3(0.f, 0.f, 0.f);
	pAngularVelocity = new XMFLOAT3(0.f, 0.f, 0.f);

	pModelClass = new BumpModelClass;
	this->pModelPath = ModelPath;
	this->pMaterialPath = MaterialPath;
	this->pMaterialPath2 = MaterialPath2;

	renderShader = RenderShader::SHADED;

	pD3DClass = 0;
	pParent = 0;
	bRotateFirst = true;
	bDontTransformParentRotation = false;
	Initialized = false;

	mCollisionRadius = 10000.0f;
	mCollisionEnabled = false;
	mHoveringEnabled = false;
	mDrawOBB = false;
	mDrawAABB = false;

	pWorld = NULL;
}

// Initialize the object, set stored materials and initialize the model

void BaseObject::Initialize(D3DClass* pD3DClass) {
	this->pD3DClass = pD3DClass;

	Initialized = true;

	if (pModelPath) {
		SetModelPath(pModelPath);
	}

	if (pMaterialPath) {
		SetMaterialPath(pMaterialPath);
	}

	if (pMaterialPath2) {
		SetNormalPath(pMaterialPath2);
	}

	if (mCollisionEnabled || mDrawOBB) {
		ComputeOBB();
	}

	if (mCollisionEnabled || mDrawAABB) {
		ComputeAABB();
	}

	if (mCollisionEnabled) {
		EnableCollisions(true);
	}
}

bool BaseObject::IsInitialized()
{
	return Initialized;
}

BaseObject::~BaseObject()
{
	if (pModelClass) {
		pModelClass->Shutdown();
		delete pModelClass;
		pModelClass = 0;
	}
}

const char* BaseObject::GetName()
{
	return Name;
}

int BaseObject::GetID() {
	return ID;
}

// this function could be overriden and used to perform custom render operations on this object

void BaseObject::OnInput(InputFrame inputFrame, float DeltaTime)
{
}

void BaseObject::OnRender(float DeltaTime)
{
	
}

void BaseObject::OnCreate()
{

}

void BaseObject::OnDestroy()
{

}

void BaseObject::Destroy()
{
	mDestroyed = true;
}

bool BaseObject::IsDestroyed()
{
	return mDestroyed;
}

bool BaseObject::operator==(BaseObject other)
{
	return ID == other.ID;
}

// Set the model path and initialize or retrieve the cached model object

void BaseObject::SetModelPath(const char* ModelPath) {
	this->pModelPath = ModelPath;

	if (!Initialized) { return; }

	bool CacheContainsModel = pWorld->ModelCache.find(ModelPath) != pWorld->ModelCache.end();
	if (CacheContainsModel) {
		BumpModelClass* pCachedModelClass = pWorld->ModelCache.at(ModelPath);
		pModelClass = pCachedModelClass;

		return;
	}

	pModelClass = new BumpModelClass;

	HRESULT result = pModelClass->Initialize(pD3DClass->GetDevice(), (char*)ModelPath, GetMaterialPath(), GetNormalPath());

	pWorld->ModelCache[ModelPath] = pModelClass;

	// Args - D3D device, model path, texture path, texture path 2
}

const char* BaseObject::GetModelPath() {
	return pModelPath;
}

void BaseObject::SetMaterialPath(WCHAR* MaterialPath) {
	this->pMaterialPath = MaterialPath;
}

WCHAR* BaseObject::GetMaterialPath() {
	return pMaterialPath;
}

void BaseObject::SetNormalPath(WCHAR* NormalPath) {
	this->pMaterialPath2 = NormalPath;
}

WCHAR* BaseObject::GetNormalPath() {
	return pMaterialPath2;
}

void BaseObject::SetParent(BaseObject* pParent) {
	this->pParent = pParent;
}

BaseObject* BaseObject::GetParent() {
	return pParent;
}

XMMATRIX TransformRotation(XMMATRIX* pMatrix, XMFLOAT3* pAngle) {
	XMMATRIX Matrix = *pMatrix;

	if (pAngle->z != 0.f) {
		Matrix = XMMatrixMultiply(Matrix, XMMatrixRotationZ(pAngle->z));
	}
	if (pAngle->y != 0.f) {
		Matrix = XMMatrixMultiply(Matrix, XMMatrixRotationY(pAngle->y));
	}
	if (pAngle->x != 0.f) {
		Matrix = XMMatrixMultiply(Matrix, XMMatrixRotationX(pAngle->x));
	}

	return Matrix;
}

XMMATRIX TransformPosition(XMMATRIX* pMatrix, XMFLOAT3* pPosition) {
	XMMATRIX Matrix = *pMatrix;

	Matrix = XMMatrixMultiply(Matrix, XMMatrixTranslation(pPosition->x, pPosition->y, pPosition->z));

	return Matrix;
}

// This recursive function is used to evaluate the world position of an object
// This function is recursive in order to take into account parent object matrices to modify the location of the child object
// This allows for one parent object to have one or more children which move along with it
// Children of a parent can also have their own children
// This is more useful for planet orbits than the city scene

XMMATRIX CalculateWorldPosition(XMMATRIX* pOrigin, BaseObject* pObject, bool dontTransformRotation) {
	XMMATRIX origin = *pOrigin;
	XMFLOAT3* pAngle = pObject->pAngle;
	XMFLOAT3* pPosition = pObject->pPosition;

	if (pObject->bRotateFirst) {
		if (!dontTransformRotation) {
			origin = TransformRotation(&origin, pAngle);
		}
		origin = TransformPosition(&origin, pPosition);
	}
	else {
		if (!dontTransformRotation) {
			origin = TransformRotation(&origin, pAngle);
		}
		origin = TransformPosition(&origin, pPosition);
	}

	if (pObject->GetParent() != 0) {
		origin = CalculateWorldPosition(&origin, pObject->GetParent(), pObject->bDontTransformParentRotation);
	}

	return origin;
}

XMMATRIX BaseObject::GetWorldMatrix(XMMATRIX origin)
{
	return GetWorldMatrix(origin, true);
}

// This returns the world matrix of the object, taking into account its parent (if it has one)
XMMATRIX BaseObject::GetWorldMatrix(XMMATRIX origin, bool useRotation) {
	
	origin = XMMatrixScaling(pScale->x, pScale->y, pScale->z);
	origin = CalculateWorldPosition(&origin, this, !useRotation);

	if (mUseOrientationMatrix) {
		return mOrientationMatrix;
	}

	return origin;
}

void BaseObject::SetScale(float scale) {
	pScale = new XMFLOAT3(scale, scale, scale);
}

void BaseObject::SetAngle(float p, float y, float r) {
	float DegToRad = 0.0174533f;

	pAngle = new XMFLOAT3(p * DegToRad, y * DegToRad, r * DegToRad);

	if (pAABBModel != 0) {
		ComputeAABB();
	}
}

XMFLOAT3 BaseObject::GetAngle() {
	float DegToRad = 0.0174533f;

	return XMFLOAT3(pAngle->x / DegToRad, pAngle->y / DegToRad, pAngle->z / DegToRad);
}

// Collision

void BaseObject::EnableCollisions(bool enabled) {
	mCollisionEnabled = enabled;

	if (pWorld != NULL && pWorld->pGraphicsClass != NULL && enabled) {
		ComputeOBB();
		ComputeAABB();

		pCollisionUtil = new CollisionUtils();		
	}
	else {
		if (!mDrawOBB && pOBB != 0) {
			pOBBModel->Shutdown();
			delete pOBBModel;
			delete pOBB;
		}
		if (!mDrawAABB && pAABB != 0) {
			pAABBModel->Shutdown();
			delete pAABBModel;
			delete pAABB;
		}
	}
}

void BaseObject::EnableHovering(bool enabled)
{
	mHoveringEnabled = enabled;
}

bool BaseObject::GetCollisionsEnabled() {
	return mCollisionEnabled;
}

bool BaseObject::GetHoveringEnabled()
{
	return mHoveringEnabled;
}

void BaseObject::ComputeOBB() {
	if (pWorld == NULL || pWorld->pGraphicsClass == NULL) { return; }

	if (pOBB != 0) {
		delete pOBB;
	}

	if (pOBBModel != 0) {
		pOBBModel->Shutdown();
		delete pOBBModel;
	}

	int vertexCount = pModelClass->GetVertexCount();

	XMFLOAT3* pMins = new XMFLOAT3(99999999,99999999, 99999999);
	XMFLOAT3* pMaxs = new XMFLOAT3(-99999999,-99999999,-99999999);

	for (int i = 0; i < vertexCount; i++) {
		ModelType* pModel = (ModelType*)(pModelClass->m_model + i);

		pMins->x = min(pModel->x, pMins->x);
		pMins->y = min(pModel->y, pMins->y);
		pMins->z = min(pModel->z, pMins->z);

		pMaxs->x = max(pModel->x, pMaxs->x);
		pMaxs->y = max(pModel->y, pMaxs->y);
		pMaxs->z = max(pModel->z, pMaxs->z);
	}
	

	pOBB = new ObjectBoundingBox(pMins, pMaxs);

	pOBBModel = new BumpModelClass;
	VertexData data = VerticesFromBoundingBox(pOBB, true);

	pOBBModel->InitializeFromVertexArray(pWorld->pGraphicsClass->m_D3D->GetDevice(), data, L"../Engine/data/white.dds");
}

void BaseObject::ComputeAABB()
{
	if (pWorld == NULL || pWorld->pGraphicsClass == NULL) { return; }

	if (pOBB == 0) {
		ComputeOBB();
	}

	if (pAABB != 0) {
		delete pAABB;
	}

	if (pAABBModel != 0) {
		pAABBModel->Shutdown();
		delete pAABBModel;
	}

	// pAABBModel = new BumpModelClass;

	XMFLOAT3* pMins = pOBB->pMins;
	XMFLOAT3* pMaxs = pOBB->pMaxs;
	XMFLOAT3 translation = XMFLOAT3(0, 0, 0);

	XMVECTOR minVector = XMLoadFloat3(pMins);
	XMVECTOR maxVector = XMLoadFloat3(pMaxs);

	XMMATRIX rotationMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	rotationMatrix = TransformRotation(&rotationMatrix, pAngle);

	BoundingBox aabb;
	BoundingBox::CreateFromPoints(aabb, minVector, maxVector);
	aabb.Transform(aabb, rotationMatrix);

	pMins = new XMFLOAT3(-aabb.Extents.x + aabb.Center.x, -aabb.Extents.y + aabb.Center.y, -aabb.Extents.z + aabb.Center.z);
	pMaxs = new XMFLOAT3(aabb.Extents.x + aabb.Center.x, aabb.Extents.y + aabb.Center.y, aabb.Extents.z + aabb.Center.z);

	// Calculate the mins and maxs of the AABB
	pMins->x = min(pMins->x, pMaxs->x);
	pMins->y = min(pMins->y, pMaxs->y);
	pMins->z = min(pMins->z, pMaxs->z);

	pMaxs->x = max(pMins->x, pMaxs->x);
	pMaxs->y = max(pMins->y, pMaxs->y);
	pMaxs->z = max(pMins->z, pMaxs->z);

	pAABB = new ObjectBoundingBox(pMins, pMaxs);

	pAABBModel = new BumpModelClass;
	VertexData data = VerticesFromBoundingBox(pAABB, true);

	pAABBModel->InitializeFromVertexArray(pWorld->pGraphicsClass->m_D3D->GetDevice(), data, L"../Engine/data/white.dds");
}

void BaseObject::DoClick() {

}

void BaseObject::DoHoverStart()
{
}

void BaseObject::DoHoverEnd()
{
}

void BaseObject::OnCollide(BaseObject * pOther, HitResult * pHitResult)
{
}

bool BaseObject::IsHovered()
{
	return mHovered;
}

void BaseObject::SetHovered(bool hovered)
{
	if (hovered != mHovered) {
		if (hovered) {
			DoHoverStart();
		}
		else {
			DoHoverEnd();
		}
	}
	
	mHovered = hovered;
}

bool BaseObject::GetDrawOBB()
{
	return mDrawOBB;
}

bool BaseObject::GetDrawAABB()
{
	return mDrawAABB;
}

void BaseObject::SetDrawOBB(bool draw)
{
	mDrawOBB = draw;

	if (draw) {
		ComputeOBB();
	}
}

void BaseObject::SetDrawAABB(bool draw)
{
	mDrawAABB = draw;

	if (draw) {
		ComputeAABB();
	}
}

HitResult* BaseObject::ResolveCollisions()
{
	std::vector<BaseObject*> objects = *pWorld->GetObjects();

	for (int i = 0; i < objects.size(); i++) {
		BaseObject* pObject = objects[i];

		if (pObject == this) { continue; }
		if (!pObject->GetCollisionsEnabled()) { continue; }

		HitResult* pHitResult = HitResult::AABB_AABB(this, pObject);
		if (pHitResult != NULL) {
			HitResult::ResolveCollision(pHitResult, this, pObject);
			OnCollide(pObject, pHitResult);
			return pHitResult;
			break;
		}
	}

	return NULL;
}
