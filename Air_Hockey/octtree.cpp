#include "Octtree.h"
using namespace Simplex;
using namespace std;
//Setting up initial values in oct tree
uint MyOctant::m_uOctantCount = 0;
//maximum of 4 levels (for now)
uint MyOctant::m_uMaxLevel = 5;
//subdivide if there is more than 8 blocks (entities = objects)
uint MyOctant::m_uIdealEntityCount = 3;
uint MyOctant::GetOctantCount(void) { return m_uOctantCount; }

//Initializing my octant
void MyOctant::Init(void)
{
	//initial vertices data will all start with 0
	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);


	//no children in the beginning 
	m_uChildren = 0;

	m_fSize = 0.0f;

	//will be used in order to increment/decrement ID when pressing page up or down
	m_uID = m_uOctantCount;
	m_uLevel = 0;

	//calling out singleton from mesh and entity manager
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	//all empty for the root and the parent
	m_pRoot = nullptr;
	m_pParent = nullptr;

	//every child at the beginning will start with the empty pointer
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

//swapping all of the content (similar as the Init) in an object
void MyOctant::Swap(MyOctant& other)
{

	swap(m_v3Center, other.m_v3Center);
	swap(m_v3Min, other.m_v3Min);
	swap(m_v3Max, other.m_v3Max);

	swap(m_uChildren, other.m_uChildren);

	swap(m_fSize, other.m_fSize);
	swap(m_uID, other.m_uID);
	swap(m_pRoot, other.m_pRoot);
	swap(m_lChild, other.m_lChild);


	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	swap(m_uLevel, other.m_uLevel);
	swap(m_pParent, other.m_pParent);

	for (uint i = 0; i < 8; i++)
	{
		swap(m_pChild[i], other.m_pChild[i]);
	}
}

//returning the parent of the object
MyOctant* MyOctant::GetParent(void)
{
	return m_pParent;
}

//this will deallocate the children members
void MyOctant::Release(void)
{
	//if the octant level is zero, there are no children.
	//to make sure, kill all of the branches that connects to children
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

//The big three

//parameterized constructor
MyOctant::MyOctant(uint maxLvl, uint numEntity)
{
	//initialize the default data
	Init();

	m_uMaxLevel = maxLvl;
	m_uIdealEntityCount = numEntity;
	m_uOctantCount = 0;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	//list of vector3 that will hold all of the minimum and maximum vectors of the Bounding Object in order to know the boundary for the parent 
	vector<vector3> minMaxList;

	uint numObj = m_pEntityMngr->GetEntityCount();

	//entity and rigidbody will be called for every number of objects
	for (uint i = 0; i < numObj; i++)
	{
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		minMaxList.push_back(pRigidBody->GetMinGlobal());
		minMaxList.push_back(pRigidBody->GetMaxGlobal());
	}

	//creating rigidbody with the lisr of all max and min vertices data
	MyRigidBody* pRigidBody = new MyRigidBody(minMaxList);

	vector3 halfWidth = pRigidBody->GetHalfWidth();
	float maxWidth = halfWidth.x;

	//compare the three half width each other with the x component and when one is bigger than the other, then update the maximum half width
	for (int i = 1; i < 3; i++)
	{
		if (maxWidth < halfWidth[i])
		{
			maxWidth = halfWidth[i];
		}
	}

	vector3 center = pRigidBody->GetCenterLocal();
	minMaxList.clear();
	SafeDelete(pRigidBody);

	//the length is the twice of the half width
	m_fSize = maxWidth * 2.0f;
	m_v3Center = center;
	//updated minimum and maximum vertices for the larger boundary
	m_v3Min = m_v3Center - (vector3(maxWidth));
	m_v3Max = m_v3Center + (vector3(maxWidth));

	//incrememting the count by 1 after updating
	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);
}
// another contructor
MyOctant::MyOctant(vector3 center, float sizeOfOctant)
{
	//initialize the default data
	Init();
	m_v3Center = center;
	m_fSize = sizeOfOctant;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	//incrememting the count by 1 after updating
	m_uOctantCount++;
}

//copy constructor
MyOctant::MyOctant(MyOctant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

//copy assignment operator
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

//destructor
MyOctant::~MyOctant()
{
	Release();
}

float MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}


//divide into 8 smaller children
void MyOctant::Subdivide(void)
{
	//don't subdivide if the current level exceeds the maximum possible level
	if (m_uLevel >= m_uMaxLevel)
	{
		return;
	}

	// don't subdivide if it is already subdivided (if the child is not 0)
	if (m_uChildren != 0)
	{
		return;
	}

	//children will now be 8
	m_uChildren = 8;

	vector3 center;
	float halfSizeCenter = m_fSize / 4.0f;
	float sizeCenter = halfSizeCenter * 2.0f;

	//staring a the left, bottom, back
	center = m_v3Center;
	center.x -= halfSizeCenter;
	center.y -= halfSizeCenter;
	center.z -= halfSizeCenter;
	m_pChild[0] = new MyOctant(center, sizeCenter);

	//second one will be at right, bottom, back
	center.x += sizeCenter;
	m_pChild[1] = new MyOctant(center, sizeCenter);

	//third one will be at right, bottom, front
	center.z += sizeCenter;
	m_pChild[2] = new MyOctant(center, sizeCenter);

	//fourth one will be at left, bottom, front
	center.x -= sizeCenter;
	m_pChild[3] = new MyOctant(center, sizeCenter);

	//fifth one will be at left, top, front
	center.y += sizeCenter;
	m_pChild[4] = new MyOctant(center, sizeCenter);

	//sixth one will be at left, top, back
	center.z -= sizeCenter;
	m_pChild[5] = new MyOctant(center, sizeCenter);

	//seventh one will be at right, top, back
	center.x += sizeCenter;
	m_pChild[6] = new MyOctant(center, sizeCenter);

	//eigth one will be at left, top, back
	center.z += sizeCenter;
	m_pChild[7] = new MyOctant(center, sizeCenter);

	//for all 8 children ...
	for (uint i = 0; i < 8; i++)
	{
		//update the level number, root and parent
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;


		//continue subdividing if there are more than entity number (8) inside the cube 
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}
	}
}

//returns the child specified in the index
MyOctant* MyOctant::GetChild(uint numChild)
{
	if (numChild > 7)
	{
		return nullptr;
	}

	return m_pChild[numChild];
}

// detecting collision
bool MyOctant::IsColliding(uint index)
{
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	//if the index exceeds the number of elements in the bounding object then there is no collision
	if (index >= nObjectCount)
	{
		return false;
	}

	//As the Octree will never rotate or scale this collision is as easy as an AARB
	//Get all vectors in global space (the octant ones are already in Global)
	MyEntity* pEntity = m_pEntityMngr->GetEntity(index);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 minObjectVertex = pRigidBody->GetMinGlobal();
	vector3 maxObjectVertex = pRigidBody->GetMaxGlobal();


	//checking collision for all X,Y,Z axis
	if (m_v3Max.x < minObjectVertex.x)
	{
		return false;
	}

	if (m_v3Min.x > maxObjectVertex.x)
	{
		return false;
	}

	if (m_v3Max.y < minObjectVertex.y)
	{
		return false;
	}

	if (m_v3Min.y > maxObjectVertex.y)
	{
		return false;
	}

	if (m_v3Max.z < minObjectVertex.z)
	{
		return false;
	}

	if (m_v3Min.z > maxObjectVertex.z)
	{
		return false;
	}

	//anything left will be detected as collision
	return true;
}

//Asks the octant if it does not contain any children
bool MyOctant::IsLeaf(void)
{
	return m_uChildren == 0;
}

//Asks the octant if it contains more than this many Bounding Objects
bool MyOctant::ContainsMoreThan(uint numEntity)
{
	uint count = 0;
	uint objCount = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < objCount; i++)
	{
		//everytime there is collision, increment the count
		if (IsColliding(i))
		{
			count++;
		}

		//if the incremented count exceeds, then the cube does contain more entity than it can hold
		if (count > numEntity)
		{
			return true;
		}
	}
	return false;
}

void MyOctant::KillBranches(void)
{
	//recursion where the method will call itself until no children is left
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}
	m_uChildren = 0;
}

//Displays the octant volume specified by index including the objects underneath
void MyOctant::Display(uint indexNum, vector3 color)
{
	if (m_uID == indexNum)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), color, RENDER_WIRE);
		return;
	}

	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(indexNum);
	}
}

//Displays the octant volume in the color specified
void MyOctant::Display(vector3 color)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), color, RENDER_WIRE);
}

//Displays the non empty leafs in the octree
void MyOctant::DisplayLeafs(vector3 color)
{
	uint nLeafs = m_lChild.size();
	for (uint i = 0; i < nLeafs; i++)
	{
		m_lChild[i]->DisplayLeafs(color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), color, RENDER_WIRE);
}

//clears the list
void MyOctant::ClearEntityList(void)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList();
	}
	m_EntityList.clear();
}

//creates the list of all leafs that contains objects
void MyOctant::ConstructList(void)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}

	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}

//Creates a tree using subdivisions, the max number of objects and levels
void MyOctant::ConstructTree(uint numMaxLevel)
{
	//if this method is tried to be applied to something else
	//other than the root, dont
	if (m_uLevel != 0)
	{
		return;
	}

	m_uMaxLevel = numMaxLevel;
	m_uOctantCount = 1;

	//ClearEntityList();
	m_EntityList.clear();

	//if the base tree
	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}

	//Add octant ID to entities
	AssignIDtoEntity();

	//construct the list of objects
	ConstructList();
}

//trace the tree back up to the leafs and sets the objects in them to the index
void MyOctant::AssignIDtoEntity(void)
{
	//trace until the leaf has been reached
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}
	if (m_uChildren == 0)
	{
		uint numEntity = m_pEntityMngr->GetEntityCount();
		for (uint i = 0; i < numEntity; i++)
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}


