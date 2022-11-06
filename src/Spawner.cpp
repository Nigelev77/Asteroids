template<typename T>
struct Spawner
{
	T m_SpawnFunc;

	Spawner(T func)
		: m_SpawnFunc{ func }
	{}

};


struct Monster
{
	float dmg;
	float health;
	float movSpeed;
};

Monster* SpawnMonster(void)
{
	return new Monster{ 10.0f, 100.0f, 1.0f };
}


typedef Monster* (*SpawnMonsterFunc)(void);

void Update()
{
	Spawner<SpawnMonsterFunc> monsterSpawner{&SpawnMonster};

	Monster* monster = monsterSpawner.m_SpawnFunc();

}