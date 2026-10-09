#pragma once

class App
{
public:
	App();
	virtual ~App();

	static App& GetInstance() { return *s_pApp; }

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

	static void MyPixelShader(cpu_ps_io& io);


	void SpawnBalls();

	void DrecreaseTimer(float dt);

	bool Collision(cpu_entity* colider, cpu_entity* colided);
	bool CollisionWithCircle(cpu_entity* colider);

private:
	inline static App* s_pApp = nullptr;

	//ressources
	cpu_font m_font;
	cpu_mesh m_meshPlayer;
	cpu_mesh m_meshCircle1;
	cpu_mesh m_meshCircle2;
	cpu_mesh m_meshBall;
	cpu_mesh m_meshShadow;

	//Shader
	cpu_material m_materialPlayer;
	cpu_material m_materialCircle1;
	cpu_material m_materialCircle2;


	//3D
	float m_playerAngle = 0.f;
	float m_playerSpeed = 1.f;
	float m_ballSpeed = 5.f;

	cpu_entity* m_pPlayer;
	cpu_entity* m_pCircle1;
	cpu_entity* m_pCircle2;
	cpu_texture m_textureGaza;


	std::unordered_map<cpu_entity*, cpu_entity*> m_balls;

	std::unordered_map<cpu_particle_emitter*, float> m_particleEmitters;

	//particle
	cpu_particle_emitter* m_pEmitter;

	//stats
	int m_HP = 3;
	int m_score = 0;
	int m_scoreValue;
	float m_basicBallSpawnCooldown = 2.f;
	float m_ballSpawnCooldown = 0.f;

	float m_basicTpCooldown = 10.f;
	float m_tpCooldown = 0.f;
	
	
	ui32 seed;

};



