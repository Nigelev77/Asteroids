// Asteroids.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Spawner.cpp"




enum ElementalInfusion
{
    ELEM_ICE, ELEM_FIRE, ELEM_WATER, ELEM_NONE
};


struct Weapon
{
    float baseDmg;
    ElementalInfusion infusion;
};


struct Ghost
{
    float dmg, health, movSpeed, transparency;
};

struct SmartMonster
{
    float dmg;
    float health;
    float movSpeed;
    Weapon weapon;
};


SmartMonster spawnSmartMonster(Weapon&& wep)
{
    //Random infusion
    wep.infusion = ELEM_WATER;
    wep.baseDmg = 100.0f;

    return { 100.0f, 500.0f, 2.5f, wep };
}

typedef SmartMonster(*SpawnSmartFunc)(Weapon&&);



Ghost* spawnGhost()
{
    return new Ghost{ 25.0f, 75.0f, 0.8f, 0.25f };
}

struct Goblin
{
    float dmg, health, movSpeed, atkSpeed;
};


Goblin* spawnGoblin(float spd)
{
    return new Goblin{ 100.0f, 25.0f, 150.0f, spd * 0.25f };
}

typedef Ghost* (*SpawnGhostFunc)(void);
typedef Goblin* (*SpawnGoblinFunc)(float);



int main()
{
    std::cout << "Hello World!\n";



    Spawner<SpawnMonsterFunc> monsterSpawner{ &SpawnMonster };
    Spawner<SpawnGhostFunc> ghostSpawner{ &spawnGhost };
    Spawner<SpawnGoblinFunc> goblinSpawner{ &spawnGoblin };
    Spawner<SpawnSmartFunc> smartMonsterSpawner{ &spawnSmartMonster };


    Monster* originMonster = monsterSpawner.m_SpawnFunc();
    Monster* monster = monsterSpawner.m_SpawnFunc();
    Ghost* ghost = ghostSpawner.m_SpawnFunc();
    Goblin* goblin = goblinSpawner.m_SpawnFunc(5.0f);
    SmartMonster smart = smartMonsterSpawner.m_SpawnFunc({ 50.f, ELEM_FIRE });


    monster->dmg = 50.0f;

    std::cout << "original monster : " << originMonster->dmg << ", " << originMonster->health << ", " << originMonster->movSpeed << "\n";
    std::cout << "edited monster : " << monster->dmg << ", " << monster->health << ", " << monster->movSpeed << "\n";
    std::cout << "ghost monster : " << ghost->dmg << ", " << ghost->health << ", " << ghost->movSpeed << ", " << ghost->transparency << "\n";
    std::cout << "goblin monster : " << goblin->dmg << ", " << goblin->health << ", " << goblin->movSpeed << ", " << goblin->atkSpeed << "\n";
    std::cout << "smart monster : " << smart.dmg << ", " << smart.health << ", " << smart.movSpeed << ", " << smart.weapon.baseDmg << ", " << smart.weapon.infusion << "\n";


    std::cin.get();
    return 0;
}

