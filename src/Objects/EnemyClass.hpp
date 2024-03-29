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

const static struct
{
    const struct
    {
        EnemyAttackMode attackMode = Kamikadze;
        int hp = 5;
        float speed = 0.8f;
        int damage_weight = 5;
        float stopOnDistance = 1;
    } kamikadze;
    const struct
    {
        EnemyAttackMode attackMode = DistanceFire;
        int hp = 60;
        float speed = 2;
        int damage_weight = 25;
        float stopOnDistance = 1;
    } winderfill;
    const struct
    {
        EnemyAttackMode attackMode = Builder;
        int hp = 200;
        float speed = 1;
        int damage_weight = 125;
        float stopOnDistance = 1;
    } mathbird;
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
    bool animInverse = true;
    SpriteRenderer *renderAlertSignal;

public:
    EKamikadze() : Enemy(), renderAlertSignal(nullptr)
    {
        healthPoint = enemy_class_info.kamikadze.hp;
    }

    void OnStart() override;

    int getDamageWeight() const override;

    void receiveDamage(int damage, float after = 0) override;

    float getStopDistance() const override;

    void OnUpdate();
};

#endif
