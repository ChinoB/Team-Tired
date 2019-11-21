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


	//create the bouncer
	m_pEntityMngr->AddEntity("AirHockey\\DSA2_AirHockey3D_Paddle_Revised_HongJ.obj", "Bouncer");
	m_pEntityMngr->UsePhysicsSolver(true, -1);
	vector3 v3Position = vector3(0.0f, 2.75f, 0.0f);
	matrix4 m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(.75f)));

	//create the table
	m_pEntityMngr->AddEntity("AirHockey\\DSA2_AirHockey3D_Table_Revised_HongJ.obj", "Table");
	//m_pEntityMngr->UsePhysicsSolver();
	v3Position = vector3(0.0f);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
	
	//create the Pieces
	m_pEntityMngr->AddEntity("AirHockey\\table_base.obj", "TableBase");
	//m_pEntityMngr->UsePhysicsSolver();
	v3Position = vector3(0.0f, 0.0f, 0.0f);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
	
	// create the puck
	m_pEntityMngr->AddEntity("AirHockey\\DSA2_AirHockey3D_Puck_Revised_HongJ.obj", "Puck");
	//m_pEntityMngr->UsePhysicsSolver();
	v3Position = vector3(0.0f, .55f, 0.0f);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(.75f)));

	//create the bumper
	m_pEntityMngr->AddEntity("AirHockey\\DSA2_AirHockey3D_Bouncer_Revised_HongJ.obj", "Bumper");
	//m_pEntityMngr->UsePhysicsSolver();
	v3Position = vector3(-1.0f, 0.468500f, -5.0f);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(.75f)));
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//The game has fixed camera so We don't need to call CameraRotation();
	//CameraRotation(); -- disabled to prevent user from rotating camera
	
	//update the position of the player's bouncer according to the position of the mouse
	MouseToWorld(m_pEntityMngr->GetEntity(0), m_pEntityMngr->GetEntity(1)); 

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list

	m_pEntityMngr->AddEntityToRenderList(-1, true);//draw puck
	
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

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