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
	m_font.Create(cpuDevice.GetHeight() <= 512 ? 14 : 28);

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
	//m_materialCircle2.color = cpu::ToColor(42, 63, 53);
	m_pCircle2 = cpuEngine.CreateEntity();
	m_textureGaza.Load("gaza.png");
	m_materialCircle2.pTexture = &m_textureGaza;

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
	m_meshShadow.CreateCircle(0.5f, 50.f, CPU_BLACK );
}

void App::OnUpdate()
{
	float dt = cpuTime.delta;

	DrecreaseTimer(dt);


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
		

	for (auto it = m_balls.begin(); it != m_balls.end(); )
	{
		std::pair<cpu_entity*, cpu_entity*> pBallAndShadow = *it;

		cpu_entity* pBall = pBallAndShadow.first;
		cpu_entity* pShadow = pBallAndShadow.second;

		pBall->transform.pos.y -= (dt * m_ballSpeed);

		if (Collision(pBall, m_pPlayer))
		{
			m_score++;
			if (m_score % 10 == 0)
			{
				m_HP++;
				m_basicBallSpawnCooldown -= 0.2f;
			}
				
			cpuEngine.Release(pBall);	
			cpuEngine.Release(pShadow);
		}

		if (CollisionWithCircle(pBall))
		{
			m_HP--;
			cpu_particle_emitter* pParticleEmmitter = m_pEmitter;
			
			pParticleEmmitter->pos = pBall->transform.pos;
			pParticleEmmitter->pos.y = pBall->transform.pos.y + pBall->pMesh->radius;

			float timer = 0.5f;

			m_particleEmitters.insert(std::pair<cpu_particle_emitter*, float>(pParticleEmmitter, timer));

			cpuEngine.Release(pBall);
			cpuEngine.Release(pShadow);
			
		}

		if (pBall->dead)
			it = m_balls.erase(it);

		else
			++it;
			
	}



	for (auto it = m_particleEmitters.begin(); it != m_particleEmitters.end();)
	{
		std::pair<cpu_particle_emitter*, float> pParticleEmitter = *it;

		if (pParticleEmitter.first->dead)
			m_particleEmitters.erase(it);

		else
			++it;
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
	switch (pass)
	{

	case CPU_PASS_UI_END:
	{
		
		std::string life = CPU_STR(m_HP) + " HP ";
		std::string score = "Score : " + CPU_STR(m_score);
		std::string teleport = "TP cooldown : " + CPU_STR((int)m_tpCooldown+1);


		XMFLOAT3 tint = { 1.0f, 1.0f, 0.8f };
		cpuDevice.DrawText(&m_font, life.c_str(), (int)(0), 10, CPU_TEXT_LEFT, &tint);
		cpuDevice.DrawText(&m_font, score.c_str(), (int)(cpuDevice.GetWidth())- 20, 10, CPU_TEXT_RIGHT, &tint);
		cpuDevice.DrawText(&m_font, teleport.c_str(), (int)(cpuDevice.GetWidth())*0.5f, 10, CPU_TEXT_CENTER, &tint);
		break;
	}
	}
}

void App::MyPixelShader(cpu_ps_io& io)
{
	// YOUR CODE HERE
	io.color = io.p.color;
}

void App::SpawnBalls()
{
	cpu_entity* pBall = cpuEngine.CreateEntity();
	pBall->pMesh = &m_meshBall;

	cpu_entity* pShadow = cpuEngine.CreateEntity();
	pShadow->pMesh = &m_meshShadow;

	m_balls.insert(std::pair<cpu_entity*, cpu_entity*> (pBall, pShadow));

	float randomAngle = XM_2PI * cpu::Rand01(seed);

	pBall->transform.OrbitAroundAxis(m_pCircle1->transform.pos, CPU_VEC3_UP, 9.8f, randomAngle);
	pBall->transform.pos.y = 20.f;

	pShadow->transform.OrbitAroundAxis(m_pCircle1->transform.pos, CPU_VEC3_UP, 9.8f, randomAngle);
	pShadow->transform.pos.y = 0.2f;
	
}

void App::DrecreaseTimer(float dt)
{

	(m_ballSpawnCooldown > 0) ? m_ballSpawnCooldown -= dt : m_ballSpawnCooldown = -1;
	(m_tpCooldown > 0) ? m_tpCooldown -= dt : m_tpCooldown = -1;

	for (auto it : m_particleEmitters)
	{
		if (it.second <= 0)
			cpuEngine.Release(it.first);

		else
			it.second -= dt;
	}
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
	float colRadius = colider->pMesh->radius;


	if (y2 - y1 >= 0+colRadius)
		return true;

	return false;
}



