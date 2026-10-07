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
	m_meshCircle1.CreateCircle(10.f, 50.f);
	m_materialCircle1.color = cpu::ToColor(0, 255, 0);
	m_pCircle1 = cpuEngine.CreateEntity();
	m_pCircle1->pMesh = &m_meshCircle1;
	m_pCircle1->pMaterial = &m_materialCircle1;
	m_pCircle1->transform.pos.z = 30.f;

	m_meshCircle2.CreateCircle(9.7f, 50.f);
	m_materialCircle2.color = cpu::ToColor(42, 63, 53);
	m_pCircle2 = cpuEngine.CreateEntity();
	m_pCircle2->pMesh = &m_meshCircle2;
	m_pCircle2->pMaterial = &m_materialCircle2;
	m_pCircle2->transform.pos.z = 29.7f;
	m_pCircle2->transform.pos.y = 0.1f;

	//particle
	cpuEngine.GetParticleData()->Create(2000000);
	cpuEngine.GetParticlePhysics()->gy = -0.5f;
	m_pEmitter = cpuEngine.CreateParticleEmitter();
	m_pEmitter->rate = 0.3f;
	m_pEmitter->colorMin = cpu::ToColor(255, 0, 0);
	m_pEmitter->colorMax = cpu::ToColor(255, 128, 0);
	m_pEmitter->durationMin = 1.f;
	m_pEmitter->durationMax = 1.f;

	//ball
	m_meshBall.CreateSphere(0.5f);
}

void App::OnUpdate()
{
	float dt = cpuTime.delta;

	DrecreaseTimer(dt);

	for (auto it : m_particleEmitter)
	{
		
		if (it.second <= 0)
		{
			cpuEngine.Release(it.first);
			continue;
		}

		it.second -= dt;
	}


	if (cpuInput.IsUp() && m_tpCooldown <= 0)
	{
		m_playerAngle += XM_PI;
		m_tpCooldown = m_basicTpCooldown;
	}
		

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
		

	for (auto it : m_balls)
	{
		cpu_entity* pBall = it;
		pBall->transform.pos.y -= (dt * m_ballSpeed);

		if (Collision(pBall, m_pPlayer))
		{
			m_score++;
			cpuEngine.Release(pBall);
			continue;
		}

		if (CollisionWithCircle(pBall))
		{
			m_HP--;
			cpuEngine.Release(pBall);
			cpu_particle_emitter* pParticleEmmitter = m_pEmitter;
			
			pParticleEmmitter->pos = pBall->transform.pos;

			float timer = 1.f;

			m_particleEmitter.insert(std::pair<cpu_particle_emitter*, float>(pParticleEmmitter, timer));
			
		}
	}


	for (auto it : m_balls)
	{
		if (it->dead)
			m_balls.erase(std::find(m_balls.begin(), m_balls.end(), it));
	}

	for (auto it : m_particleEmitter)
	{
		if (it.first->dead)
			m_particleEmitter.erase(std::find(m_balls.begin(), m_balls.end(), it));
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
	cpu_entity* pBalls = cpuEngine.CreateEntity();
	pBalls->pMesh = &m_meshBall;
	m_balls.push_back(pBalls);

	float randomAngle = XM_2PI * cpu::Rand01(seed);

	pBalls->transform.OrbitAroundAxis(m_pCircle1->transform.pos, CPU_VEC3_UP, 9.8f, randomAngle);
	pBalls->transform.pos.y = 20.f;
	
}

void App::DrecreaseTimer(float dt)
{

	m_ballSpawnCooldown -= dt;
	m_tpCooldown -= dt;
}

bool App::Collision(cpu_entity* colider, cpu_entity* colided)
{
	float colliderRadius = colider->pMesh->radius;

	float collidedRadius = colided->pMesh->radius;

	float x1 = colider->transform.pos.x;
	float y1 = colider->transform.pos.y;
	float z1 = colider->transform.pos.z;
	
	float x2 = colided->transform.pos.x;
	float y2 = colided->transform.pos.y;
	float z2 = colided->transform.pos.z;

	float xTot = (x2 - x1) * (x2 - x1);
	float yTot = (y2 - y1) * (y2 - y1);
	float zTot = (z2 - z1) * (z2 - z1);

	float distanceWplayer = sqrt((xTot + yTot + zTot));

	if (distanceWplayer <= collidedRadius + colliderRadius)
		return true;

	return false;
}

bool App::CollisionWithCircle(cpu_entity* colider)
{
	float y1 = colider->transform.pos.y;
	float y2 = m_pCircle1->transform.pos.y;

	if (y2 - y1 >= 0)
		return true;

	return false;
}
