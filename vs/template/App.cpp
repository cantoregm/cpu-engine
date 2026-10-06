#include "pch.h"

App::App()
{
	s_pApp = this;
	CPU_CALLBACK_START(OnStart);
	CPU_CALLBACK_UPDATE(OnUpdate);
	CPU_CALLBACK_EXIT(OnExit);
	CPU_CALLBACK_RENDER(OnRender);
}

App::~App()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void App::OnStart()
{
	//seed 

	seed = (ui32)timeGetTime();

	//kadmérad
	
	cpuEngine.GetCamera()->transform.SetYPR(0.f, 0.35f);
	cpuEngine.GetCamera()->transform.pos.y = 10.f;
	cpuEngine.GetCamera()->transform.pos.z = 5.f;


	//player
	m_meshPlayer.CreateCube(0.5f);
	m_materialPlayer.color = cpu::ToColor(255, 255, 0);
	m_pPlayer = cpuEngine.CreateEntity();
	m_pPlayer->pMesh = &m_meshPlayer;
	m_pPlayer->pMaterial = &m_materialPlayer;
	m_pPlayer->transform.pos.z = 23.f;

	//cicle
	m_meshCircle1.CreateCircle(10.f, 20.f);
	m_materialCircle1.color = cpu::ToColor(0, 255, 0);
	m_pCircle1 = cpuEngine.CreateEntity();
	m_pCircle1->pMesh = &m_meshCircle1;
	m_pCircle1->pMaterial = &m_materialCircle1;
	m_pCircle1->transform.pos.z = 30.f;

	m_meshCircle2.CreateCircle(9.7f, 20.f);
	m_materialCircle2.color = cpu::ToColor(42, 63, 53);
	m_pCircle2 = cpuEngine.CreateEntity();
	m_pCircle2->pMesh = &m_meshCircle2;
	m_pCircle2->pMaterial = &m_materialCircle2;
	m_pCircle2->transform.pos.z = 29.7f;
	m_pCircle2->transform.pos.y = 0.1f;

	//ball
	m_meshBall.CreateSphere(2.f);
}

void App::OnUpdate()
{
	DrecreaseTimer();

	float dt = cpuTime.delta;

	if (cpuInput.IsLeft())
		m_playerAngle += dt * XM_PI * m_playerSpeed;

	if (cpuInput.IsRight())
		m_playerAngle -= dt * XM_PI * m_playerSpeed;


	m_pPlayer->transform.OrbitAroundAxis(m_pCircle1->transform.pos, CPU_VEC3_UP, 9.8f, m_playerAngle);

	if (m_ballSpawnCooldown <= 0)
	{
		m_ballSpawnCooldown = m_basicBallSpawnCooldown;
		SpawnBalls();
	}
		


	for (auto it = m_balls.begin(); it != m_balls.end(); ++it)
	{
		cpu_entity* pBall = *it;
		pBall->transform.pos.y -= (dt * m_ballSpeed);
	}

	if (cpuInput.IsBackPressed())
		cpuEngine.Quit();
	
}

void App::OnExit()
{
	// YOUR CODE HERE
}

void App::OnRender(int pass)
{
	// YOUR CODE HERE
}

void App::MyPixelShader(cpu_ps_io& io)
{
	// YOUR CODE HERE
	io.color = io.p.color;
}

void App::SpawnBalls()
{
	cpu_entity* pMissile = cpuEngine.CreateEntity();
	pMissile->pMesh = &m_meshBall;
	pMissile->transform.SetScaling(0.2f);
	m_balls.push_back(pMissile);

	float randomAngle = XM_2PI * cpu::Rand01(seed);

	pMissile->transform.OrbitAroundAxis(m_pCircle1->transform.pos, CPU_VEC3_UP, 9.8f, randomAngle);
	pMissile->transform.pos.y = 20.f;
	
}

void App::DrecreaseTimer()
{
	float dt = cpuTime.delta;

	m_ballSpawnCooldown -= dt;
}

void App::CollisionAndReaction()
{
	float playerRadius = m_pPlayer->pMesh->radius;
	
	for (auto it = m_balls.begin(); it != m_balls.end(); ++it)
	{
		cpu_entity* pBall = *it;
		float ballRadius = pBall->pMesh->radius;

		float x1 = pBall->transform.pos.x;
		float x2 = m_pPlayer->transform.pos.x;
		float y1 = pBall->transform.pos.y;
		float y2 = m_pPlayer->transform.pos.y;
		float z1 = pBall->transform.pos.z;
		float z2 = m_pPlayer->transform.pos.z;

		float xTot = (x2 - x1)* (x2 - x1);
		float yTot = (y2 - y1) * (y2 - y1);
		float zTot = (z2 - z1) * (z2 - z1);

		float distanceWplayer = sqrt((xTot + yTot + zTot));

		float yCircle = m_pCircle1->transform.pos.y;

		float distanceWcircle = abs((y2 - y1));

		if (distanceWplayer <= playerRadius + ballRadius)
			MACRON EXPLOSION n,lkjfkmljfbljnhlkgfnl km
		
	}
}
