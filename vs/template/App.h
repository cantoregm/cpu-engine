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

	void DrecreaseTimer();

private:
	inline static App* s_pApp = nullptr;

	//ressources
	cpu_font m_font;
	cpu_mesh m_meshPlayer;
	cpu_mesh m_meshCircle1;
	cpu_mesh m_meshCircle2;
	cpu_mesh m_meshBall;

	//Shader
	cpu_material m_materialPlayer;
	cpu_material m_materialCircle1;
	cpu_material m_materialCircle2;

	//3D
	float m_playerAngle = 0.f;
	float m_playerSpeed = 0.5f;
	float m_ballSpeed = 10.f;

	cpu_entity* m_pPlayer;
	cpu_entity* m_pbonus;
	cpu_entity* m_ppoint;
	cpu_entity* m_pCircle1;
	cpu_entity* m_pCircle2;

	std::list<cpu_entity*> m_balls;

	//stats
	int m_HP;
	int m_score;
	int m_scoreValue;
	float m_basicBallSpawnCooldown = 2.f;
	float m_ballSpawnCooldown = 0.f;
	
	ui32 seed;
	

};



