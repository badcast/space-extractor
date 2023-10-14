#include "EnemyClass.hpp"
#include "Worlds/WGame.hpp"

static Sprite *sprite_explode = nullptr;
static Sprite *sprite_explode_flow = nullptr;

Particle *putParticleExplode(Vec2 position)
{
    if(sprite_explode == nullptr)
    {
        sprite_explode = Primitive::create_sprite2d_from(WGame::spriteAsset->GetImage("explode-v1"));
    }

    if(sprite_explode_flow == nullptr)
    {
        sprite_explode_flow = Primitive::create_sprite2d_from(WGame::spriteAsset->GetImage("alert-place-flow"));
    }

    // PARTICLE FIRE
    Particle *particle = Primitive::create_empty_game_object(position)->AddComponent<Particle>();
    particle->loop = false;
    particle->speed = 2;
    particle->source = sprite_explode;
    particle->direction = Vec2::zero;
    particle->maxParticles = 1;
    particle->setInterpolates(2, 0.05f, 0.3f);
    particle->setColors(Color::white, Color::transparent);
    particle->setSizes(Vec2::half / 4, Vec2::half / 2);

    // PARTICLE DROPS DRAINS
    particle = Primitive::create_empty_game_object(position)->AddComponent<Particle>();
    particle->loop = false;
    particle->randomDirection = true;
    particle->speed = 0.4f;
    particle->source = Primitive::create_sprite2D_box(Vec2::one, Color::white);
    particle->interval = 0.1f;
    particle->maxParticles = 4;
    particle->setInterpolates(1);
    particle->setColors({Color::white,100}, Color::darkgray, Color::transparent);
    particle->setSizes(Vec2::one / 16, Vec2::one / 14);

    // PARTICLE RECTANGLE
    particle = Primitive::create_empty_game_object(position)->AddComponent<Particle>();
    particle->loop = false;
    particle->rotate = false;
    particle->destroyAfter = true;
    particle->source = sprite_explode_flow;
    particle->direction = Vec2::zero;
    particle->maxParticles = 1;
    particle->speed = 5;
    particle->setInterpolates(0.3f, 0.1f, 0.5f);
    particle->setColors(Color::transparent, Color::white, Color::transparent);
    particle->setSizes(Vec2::zero, Vec2::one * 2);
    return particle;
}

void Enemy::moveTo(Vec2 targetPoint)
{
}

void EKamikadze::OnStart()
{
    transform()->layer(Layers::EnemyClass);
    SpriteRenderer *spriteRender = AddComponent<SpriteRenderer>();
    spriteRender->setSprite(Primitive::create_sprite2d_from(WGame::spriteAsset->GetImage("enemy-low")));
    spriteRender->setSize(spriteRender->getSize() / 10);

    startPoint = transform()->position();
    alertEnemySignal = Primitive::create_empty_game_object({Vec2::up / 4.4f})->AddComponent<SpriteRenderer>();
    alertEnemySignal->setSprite(Primitive::create_sprite2d_from(WGame::spriteAsset->GetImage("alert-enemy")));
    alertEnemySignal->transform()->setParent(transform(), false);
    alertEnemySignal->setColor(Color::red);
    alertEnemySignal->setSize(Vec2::half);
    alertEnemySignal->gameObject()->SetActive(false);
}

int EKamikadze::getDamageWeight() const
{
    return enemy_class_info.kamikadze.damage_weight;
}

void EKamikadze::receiveDamage(int damage)
{
    healthPoint = Math::Max(0, healthPoint - damage);

    if(healthPoint == 0)
    {
        putParticleExplode(this->transform()->position());
        Destroy(this->gameObject()); // can destroy
    }
}

void EKamikadze::OnUpdate()
{
    Vec2 target_to;
    float distance;
    Player *player = WGame::current->player;
    if(reverse)
    {
        target_to = player->transform()->position();
        distance = 2;
    }
    else
    {
        target_to = startPoint;
        distance = 0.1;
    }
    alertEnemySignal->transform()->angle(alertEnemySignal->transform()->angle() + 2);
    transform()->LookAt(player->transform());

    transform()->position(
        Vec2::MoveTowards(transform()->position(), target_to, TimeEngine::deltaTime() * enemy_class_info.kamikadze.speed));

    float _remainedDistance = Vec2::Distance(transform()->position(), target_to);

    if(Input::GetMouseUp(MouseState::MouseRight))
    {
        putParticleExplode(transform()->position());
        this->gameObject()->Destroy();
    }
    else if(_remainedDistance < distance)
    {
        if(!reverse)
        {
            startPoint -= Random::RandomVector();
        }
        else
        {
            if(1 || Random::Range(0, 10) < 5)
            {
                // SUICIDE
                player->applyDamage(enemy_class_info.kamikadze.damage_weight);
                putParticleExplode(transform()->position());
                Destroy(gameObject());
            }
        }

        reverse = !reverse;
    }
    else if(_remainedDistance < distance * 3)
    {
        if(!alertEnemySignal->gameObject()->isActive())
            alertEnemySignal->gameObject()->SetActive(true);
        constexpr int alert_value = 10;
        // Animate color
        Color color = alertEnemySignal->getColor();
        if(!animInverse)
            animInverse = ((color.a += alert_value) == 255);
        else
            animInverse = !((color.a -= alert_value) == 0);
        alertEnemySignal->setColor(color);
    }
}
