 #include "World.h"

#include "StellarBody.h"

World::World()
{
	CurrentID = 0;
	Objects = new std::vector<BaseObject*>();

	pCameraPosition = new XMFLOAT3(0.f, 0.f, -10.f);

	BaseObject* TestObject = CreateObject<BaseObject>("Test Object", "../Engine/data/cube.txt", "../Engine/data/stone.dds", "../Engine/data/normal.dds");
	TestObject->pPosition = new XMFLOAT3(0, 30, 100);
	//TestObject->pVelocity->x = 3.f;

	/*BaseObject* TestChild = CreateObject<BaseObject>("Test Child", "../Engine/data/cube.txt", "../Engine/data/stone.dds", "../Engine/data/normal.dds");
	TestChild->pPosition = new XMFLOAT3(0, 5, 0);
	TestChild->SetParent(TestObject);
	TestChild->pAngularVelocity = new XMFLOAT3(0, 0, 2);
	TestChild->bRotateFirst = false;

	BaseObject* TestChild2 = CreateObject<BaseObject>("Test Child 2", "../Engine/data/cube.txt", "../Engine/data/stone.dds", "../Engine/data/normal.dds");
	TestChild2->pPosition = new XMFLOAT3(0, 3, 0);
	TestChild2->SetParent(TestChild);
	TestChild2->pAngularVelocity = new XMFLOAT3(0, 5, 0);
	TestChild2->bRotateFirst = false;*/

	StellarBody* Planet = CreateObject<StellarBody>("Test planet 1", "../Engine/data/sphere_hd.obj", "../Engine/data/stone.dds", "../Engine/data/normal.dds");
	Planet->SetParent(TestObject);
	Planet->OrbitDistance = 15.f;
	Planet->OrbitSpeed = 0.f;
	Planet->bDontTransformParentRotation = true;
	//Planet->pAngularVelocity->y = 0.5f;
	
	StellarBody* Planet2 = CreateObject<StellarBody>("Test planet 2", "../Engine/data/sphere_hd.obj", "../Engine/data/stone.dds", "../Engine/data/normal.dds");
	Planet2->SetParent(Planet);
	Planet2->OrbitDistance = 50.f;
	Planet2->OrbitSpeed = 85.f;
	Planet2->pAngularVelocity->y = 10.f;
	Planet2->bDontTransformParentRotation = true;

	StellarBody* Planet3 = CreateObject<StellarBody>("Test planet 3", "../Engine/data/sphere_hd.obj", "../Engine/data/stone.dds", "../Engine/data/normal.dds");
	Planet3->SetParent(Planet2);
	Planet3->OrbitDistance = 40.f;
	Planet3->OrbitSpeed = 235.f;
	Planet3->bDontTransformParentRotation = true;
	
}


World::~World()
{
	delete Objects;
}

std::vector<BaseObject*>* World::GetObjects()
{
	return Objects;
}

template<class T>
T* World::CreateObject(const char* Name, const char* ModelPath, const char* MaterialPath, const char* MaterialPath2)
{
	T* pObject = new T(Name, ModelPath, MaterialPath, MaterialPath2);
	pObject->ID = CurrentID;
	CurrentID++;

	// Add to object array & call create functions
	Objects->push_back((BaseObject*)pObject);

	pObject->OnCreate();

	return pObject;
}

void World::DestroyObject(BaseObject* pObject)
{
	// Remove from object array & call destroy functions
	std::vector<BaseObject*>::iterator index = std::find(Objects->begin(), Objects->end(), pObject);
	if (index != Objects->end()) {
		Objects->erase(index);
	}

	pObject->OnDestroy();

	delete pObject;
}