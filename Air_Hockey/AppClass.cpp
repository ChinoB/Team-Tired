#include "AppClass.h"
#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 10.0f, 14.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)


	//create the paddle
	m_pEntityMngr->AddEntity("AirHockey\\DSA2_AirHockey3D_Paddle_Revised_HongJ.obj", "Paddle");
	m_pEntityMngr->UsePhysicsSolver(true, -1);
	m_pEntityMngr->GetEntity(-1)->SetMass(1.0f);
	vector3 v3Position = vector3(0.0f, 2.75f, 2.0f);
	matrix4 m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(.65f)));
	m_pEntityMngr->GetEntity(-1)->SetTag("Paddle");

	//create the various pieces of the table
	{
		//create the Pieces
		m_pEntityMngr->AddEntity("AirHockey\\table_base_r.obj", "MousePlane");
		m_pEntityMngr->UsePhysicsSolver(true, -1);
		m_pEntityMngr->GetEntity(-1)->SetMass(1000.0f);
		v3Position = vector3(0.0f, -0.0f, 0.0f);
		m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(50.0f, 1.0f, 50.0f)));
		m_pEntityMngr->GetEntity(-1)->SetTag("MousePlane");
		//create the Pieces
		m_pEntityMngr->AddEntity("AirHockey\\table_base_r.obj", "TableBase");
		m_pEntityMngr->UsePhysicsSolver(true, -1);
		m_pEntityMngr->GetEntity(-1)->SetMass(1000.0f);
		v3Position = vector3(0.0f, 0.0f, 0.0f);
		m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.50f)));
		m_pEntityMngr->GetEntity(-1)->SetTag("TableBase");
		//create the Pieces
		m_pEntityMngr->AddEntity("AirHockey\\table_wall_N.obj", "TableWallN");
		m_pEntityMngr->UsePhysicsSolver(true, -1);
		m_pEntityMngr->GetEntity(-1)->SetMass(1000.0f);
		v3Position = vector3(0.0f, 0.0f, 0.0f);
		m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.50f)));
		m_pEntityMngr->GetEntity(-1)->SetTag("Wall");
		//create the Pieces
		m_pEntityMngr->AddEntity("AirHockey\\table_wall_S.obj", "TableWallS");
		m_pEntityMngr->UsePhysicsSolver(true, -1);
		m_pEntityMngr->GetEntity(-1)->SetMass(1000.0f);
		v3Position = vector3(0.0f, 0.0f, 0.0f);
		m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.50f)));
		m_pEntityMngr->GetEntity(-1)->SetTag("Wall");
		//create the Pieces
		m_pEntityMngr->AddEntity("AirHockey\\table_wall_E.obj", "TableWallE");
		m_pEntityMngr->UsePhysicsSolver(true, -1);
		m_pEntityMngr->GetEntity(-1)->SetMass(1000.0f);
		v3Position = vector3(0.0f, 0.0f, 0.0f);
		m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.50f)));
		m_pEntityMngr->GetEntity(-1)->SetTag("Wall");
		//create the Pieces
		m_pEntityMngr->AddEntity("AirHockey\\table_wall_W.obj", "TableWallW");
		m_pEntityMngr->UsePhysicsSolver(true, -1);
		m_pEntityMngr->GetEntity(-1)->SetMass(1000.0f);
		v3Position = vector3(0.0f, 0.0f, 0.0f);
		m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.50f)));
		m_pEntityMngr->GetEntity(-1)->SetTag("Wall");
	}

	//create the bumper
	m_pEntityMngr->AddEntity("AirHockey\\DSA2_AirHockey3D_Bouncer_Revised_HongJ.obj", "Bumper");
	m_pEntityMngr->UsePhysicsSolver();
	v3Position = vector3(-2.0f, .4650f, -2.0f);
	m_pEntityMngr->GetEntity(-1)->SetMass(100.0f);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(.33f)));
	m_pEntityMngr->GetEntity(-1)->SetTag("Bumper");

	//create the bumper
	m_pEntityMngr->AddEntity("AirHockey\\DSA2_AirHockey3D_Bouncer_Revised_HongJ.obj", "Bumper");
	m_pEntityMngr->UsePhysicsSolver();
	v3Position = vector3(2.0f, .4650f, -2.0f);
	m_pEntityMngr->GetEntity(-1)->SetMass(100.0f);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(.33f)));
	m_pEntityMngr->GetEntity(-1)->SetTag("Bumper");
	
	// create the puck
	addPuck();

	// vectors for the collision space
	maxTable = m_pEntityMngr->GetEntity(2)->GetRigidBody()->GetMaxGlobal();// -2.5f;
	minTable = m_pEntityMngr->GetEntity(2)->GetRigidBody()->GetMinGlobal();// +2.5f;

	float spaceTop = m_pEntityMngr->GetEntity(3)->GetRigidBody()->GetMaxGlobal().y;
	float spaceBottom = m_pEntityMngr->GetEntity(3)->GetRigidBody()->GetMinGlobal().y;
	
	maxTable.y = spaceTop;
	minTable.y = spaceBottom;

	printf("Table min: [%f, %f, %f]\n", minTable.x, minTable.y, minTable.z);
	printf("Table max: [%f, %f, %f]\n", maxTable.x, maxTable.y, maxTable.z);

	// create octree (with only one t)
	m_pEntityMngr->Update();

	//create clock for delta time
	m_uClock = m_pSystem->GenClock();
}
void Application::Update(void)
{
	m_uOctantLevels = 3;
	m_pRoot = new MyOctant(m_uOctantLevels, 3, minTable, maxTable);

	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//The game has fixed camera so We don't need to call CameraRotation();
	//CameraRotation(); //-- disabled to prevent user from rotating camera
	

	//add new puck every X seconds
	if (m_fCurrent >= m_fDelay) {
		addPuck();
		m_fCurrent = 0;
	}
	else {
		m_fCurrent += m_pSystem->GetDeltaTime(m_uClock);
	}

	//update the position of the player's paddle according to the position of the mouse
	MouseToWorld(m_pEntityMngr->GetEntity(0), m_pEntityMngr->GetEntity(1)); 

	//check if the puck is out of bounds of the board
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		if (m_pEntityMngr->GetEntity(i)->GetTag() == "Puck")
		{
			vector3 puckPosition = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetCenterGlobal();

			//place puck back in table x
			if (maxTable.x < puckPosition.x)
				puckPosition.x = maxTable.x;
			if (minTable.x > puckPosition.x)
				puckPosition.x = minTable.x;
			
			//place puck back on table z
			if (maxTable.z < puckPosition.z)
				puckPosition.z = maxTable.z;
			if (minTable.z > puckPosition.z)
				puckPosition.z = minTable.z;

			m_pEntityMngr->GetEntity(i)->SetPosition(puckPosition);
		}
	}

	//Add puck if there are none
	bool thereIsAPuck = false;
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		if (m_pEntityMngr->GetEntity(i)->GetTag() == "Puck")
			thereIsAPuck = true;
	}
	if (!thereIsAPuck)
		addPuck();

	//Update Entity Manager
	m_pEntityMngr->Update();

	m_pRoot->AssignIDtoEntity();
	//m_pRoot->ConstructTree(m_uOctantLevels);

	//Render everything except the MousePlane
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		if(i != 1 && showQuadtree)
			m_pEntityMngr->AddEntityToRenderList(i, true);//draw everything
		else
			m_pEntityMngr->AddEntityToRenderList(i, false);//draw everything
	}
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//display octree
	if(showQuadtree == true)
	{
		m_pRoot->Display();
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}