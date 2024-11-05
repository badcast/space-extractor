/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
 */

#ifndef _ENEMY_CLASS_HPP_
#define _ENEMY_CLASS_HPP_

#include "SpaceDepends.hpp"

enum EnemyAttackMode
{
    Kamikadze,
    DistanceFire,
    Builder,
};

struct EnemyCharacter
{
    EnemyAttackMode attackMode;
    int hp;
    float rotateSpeed;
    float speed;
    int damage_weight;
    float stopOnDistance;
};

extern struct EnemyClasses
{
    EnemyCharacter kamikadze;
    EnemyCharacter winderfill;
    EnemyCharacter mathbird;
} enemy_class_info;

class Enemy : public Behaviour
{
protected:
    Vec2 targetTo;

public:
    int healthPoint;
    Vec2 startPoint;

    Enemy() : Behaviour("Enemy"), healthPoint(0)
    {
    }

    // Movement Enemy to point
    void moveTo(Vec2 targetPoint);
    // Enemy damage weight
    virtual int getDamageWeight() const = 0;
    // Receiving damage
    virtual void receiveDamage(int damage, float after = 0) = 0;
    // Result stop-move of distance (From -> To)
    virtual float getStopDistance() const = 0;
};

class EKamikadze : public Enemy
{
private:
    float animInverse = true;
    SpriteRendererRef renderAlertSignal;

public:
    EKamikadze() : Enemy(), renderAlertSignal(nullptr)
    {
        healthPoint = enemy_class_info.kamikadze.hp;
    }

    void OnAwake() override;
    void OnUpdate() override;

    int getDamageWeight() const override;

    void receiveDamage(int damage, float after = 0) override;

    float getStopDistance() const override;
};

#endif
