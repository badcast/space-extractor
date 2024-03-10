#include "EnemyClass.hpp"
#include "Worlds/WGame.hpp"

using namespace RoninEngine::Runtime;

static Sprite *sprite_explode = nullptr;
static Sprite *sprite_explode_flow = nullptr;
static Sprite *sprite_drop_drains = nullptr;
static Sprite *sprite_drop_drains2 = nullptr;

ParticleSystem *putEnemyParticleExplode(Vec2 position)
{
    if(sprite_explode == nullptr)
    {
        sprite_explode = Primitive::CreateSpriteFrom(spriteAsset->GetImage("explode-v1"), false);
    }

    if(sprite_explode_flow == nullptr)
    {
        sprite_explode_flow = Primitive::CreateSpriteFrom(spriteAsset->GetImage("alert-place-flow"), false);
    }

    if(sprite_drop_drains == nullptr)
    {
        sprite_drop_drains = Primitive::CreateSpriteTriangle(false);
    }

    if(sprite_drop_drains2 == nullptr)
    {
        sprite_drop_drains2 = Primitive::CreateSpriteRectangle(false);
    }

    // PARTICLE FIRE
    ParticleSystem *particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->speed = 2;
    particle->direction = Vec2::zero;
    particle->setSource(sprite_explode);
    particle->setLimit(1);
    particle->setInterpolates(4, 0.05f, 0.1f);
    particle->setColors(Color::white, Color::white, Color::transparent);
    particle->setSizes(Vec2::half / 4, Vec2::half / 2);

    // PARTICLE DROPS DRAINS
    particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->randomDirection = true;
    particle->speed = 0.4f;
    particle->interval = 0.5f;
    particle->startWith = 5;
    particle->setSources(sprite_drop_drains, sprite_drop_drains2);
    particle->setInterpolates(3);
    particle->setColors({Color::white, 100}, Color::darkgray, Color::transparent);
    particle->setSizes(Vec2::one / 11, Vec2::one / 15);

    // PARTICLE RECTANGLE
    particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->rotate = false;
    particle->destroyAfter = true;
    particle->direction = Vec2::zero;
    particle->speed = 5;
    particle->setLimit(1);
    particle->setInterpolates(3, 0.1f, 0.1f);
    particle->setSource(sprite_explode_flow);
    particle->setColors(Color::transparent, Color::white, Color::transparent);
    particle->setSizes(Vec2::zero, Vec2::one * 2);
    return particle;
}

void Enemy::moveTo(Vec2 targetPoint)
{
    this->targetTo = targetPoint;
}

void EKamikadze::OnStart()
{
    transform()->layer(GameLayers::EnemyClass);
    SpriteRenderer *spriteRender = AddComponent<SpriteRenderer>();
    spriteRender->renderOrder = RenderOrder::EnemyOrder;
    spriteRender->setSprite(Primitive::CreateSpriteFrom(spriteAsset->GetImage("enemy-low")));
    spriteRender->setSize(spriteRender->getSize() / 10);

    startPoint = transform()->position();
    renderAlertSignal = Primitive::CreateEmptyGameObject({Vec2::up / 4.4f})->AddComponent<SpriteRenderer>();
    renderAlertSignal->setSprite(Primitive::CreateSpriteFrom(spriteAsset->GetImage("alert-enemy")));
    renderAlertSignal->transform()->setParent(transform(), false);
    renderAlertSignal->setColor(Color::red);
    renderAlertSignal->setSize(Vec2::half);
    renderAlertSignal->gameObject()->SetActive(false);
}

int EKamikadze::getDamageWeight() const
{
    return enemy_class_info.kamikadze.damage_weight;
}

void EKamikadze::receiveDamage(int damage, float after)
{
    healthPoint = Math::Max(0, healthPoint - damage);

    if(healthPoint == 0)
    {
        putEnemyParticleExplode(this->transform()->position());
        Destroy(this->gameObject(), after); // can destroy
    }
}

float EKamikadze::getStopDistance() const
{
    return enemy_class_info.kamikadze.stopOnDistance;
}

void EKamikadze::OnUpdate()
{
    float distance;
    Player *player = WGame::current->player;

    distance = 2;
    renderAlertSignal->transform()->angle(renderAlertSignal->transform()->angle() + 2);
    transform()->LookAt(player->transform());

    transform()->position(Vec2::MoveTowards(transform()->position(), targetTo, TimeEngine::deltaTime() * enemy_class_info.kamikadze.speed));

    float currentDistance = Vec2::Distance(transform()->position(), targetTo);

    if(currentDistance <= getStopDistance())
    {
    }

    if(currentDistance < distance)
    {
        // SUICIDE
        player->applyDamage(getDamageWeight(), transform()->position());
        putEnemyParticleExplode(transform()->position());
        Destroy(gameObject());
    }
    else if(currentDistance < distance * 3)
    {
        constexpr int alert_value = 10;

        if(!renderAlertSignal->gameObject()->isActive())
            renderAlertSignal->gameObject()->SetActive(true);

        // Animate color
        Color color = renderAlertSignal->getColor();
        if(!animInverse)
            animInverse = ((color.a += alert_value) == 255);
        else
            animInverse = !((color.a -= alert_value) == 0);
        renderAlertSignal->setColor(color);
    }
}
