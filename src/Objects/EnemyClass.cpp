#include "EnemyClass.hpp"
#include "Worlds/WGame.hpp"

using namespace RoninEngine::Runtime;

/*
    int hp;
    float rotateSpeed;
    float speed;
    int damage_weight;
    float stopOnDistance;
*/
EnemyClasses enemy_class_info {{Kamikadze, 1, 1, 0.8f, 5, 1.0f}, {DistanceFire, 1, 60, 2.0f, 25, 1.0f}, {Builder, 1, 200, 1.0f, 125, 1.0f}};

class ParticleMoveOut : public Behaviour
{
public:
    float start;
    ParticleSystemRef particle;

    void OnStart() override
    {
        start = Time::time() + 2;
        particle->rotatePerFrame = 0;
    }

    void OnUpdate() override
    {
        if(Time::time() < start)
            return;

        Vec2 to;
        to = WGame::current->player->transform()->position();
        particle->rotatePerFrame = 120;
        if(transform()->position() == to)
        {
            if(!World::GetCurrentWorld()->StateObjectDestruction(this->gameObject()))
                this->gameObject()->Destroy(0.3f);
            return;
        }

        transform()->position(Vec2::MoveTowards(transform()->position(), to, Time::deltaTime() * 5));
    }
};

ParticleSystemRef putEnemyParticleExplode(Vec2 position)
{

    if(WGame::current->sprite_explode == nullptr)
        WGame::current->sprite_explode = globalAssets.gameSprites->GetSprite("explode-v1");
    if(WGame::current->sprite_explode_flow == nullptr)
        WGame::current->sprite_explode_flow = globalAssets.gameSprites->GetSprite("alert-place-flow");
    if(WGame::current->sprite_drop_drains == nullptr)
        WGame::current->sprite_drop_drains = Primitive::CreateSpriteTriangle();
    if(WGame::current->sprite_drop_drains2 == nullptr)
        WGame::current->sprite_drop_drains2 = Primitive::CreateSpriteRectangle();

    // PARTICLE FIRE
    ParticleSystemRef particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->speed = 2;
    particle->direction = Vec2::zero;
    particle->rotatePerFrame = 0;
    particle->worldSpace = false;
    particle->setSource(WGame::current->sprite_explode);
    particle->setLimit(1);
    particle->setInterpolates(10);
    particle->setColors({Color::white, 100}, Color::white, {Color::white, 0});
    particle->setSize(Vec2::half / 4);
    particle->AddComponent<ParticleMoveOut>()->particle = particle;
    particle->transform()->zOrder(RenderOrders::ParticlesOrder);

    // PARTICLE DROPS DRAINS
    particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->randomDirection = true;
    particle->speed = 0.2f;
    particle->interval = 0.5f;
    particle->startWith = 3;
    particle->setLimit(3);
    particle->setSources(globalAssets.artefacts->GetAtlasObject()->GetSprites());
    particle->setInterpolates(3);
    particle->setColors({Color::darkgreen, 100}, {Color::white, 200}, {Color::white, 0});
    particle->setSizes(Vec2::one / 11, Vec2::one / 15);
    particle->transform()->zOrder(RenderOrders::ParticlesOrder);

    particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->randomDirection = true;
    particle->speed = 0.2f;
    particle->interval = 0.5f;
    particle->startWith = 3;
    particle->setLimit(5);
    particle->setSource(WGame::current->sprite_drop_drains);
    particle->setInterpolates(3);
    particle->setColors(Color::red, Color::transparent);
    particle->setSizes(Vec2::one / 8, Vec2::one / 10);
    particle->transform()->zOrder(RenderOrders::ParticlesOrder);

    // PARTICLE RECTANGLE
    particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->rotate = false;
    particle->destroyAfter = true;
    particle->direction = Vec2::zero;
    particle->speed = 5;
    particle->setLimit(1);
    particle->setInterpolates(3, 0.1f, 0.1f);
    particle->setSource(WGame::current->sprite_explode_flow);
    particle->setColors(Color::transparent, Color::white, Color::transparent);
    particle->setSizes(Vec2::one / 2, Vec2::one / 3);
    particle->transform()->zOrder(RenderOrders::ParticlesOrder);
    return particle;
}

void Enemy::moveTo(Vec2 targetPoint)
{
    this->targetTo = targetPoint;
}

void EKamikadze::OnAwake()
{
    SpriteRendererRef spriteRender;

    startPoint = transform()->position();
    transform()->layer(GameLayers::EnemyClass);
    transform()->zOrder(RenderOrders::EnemyOrder);

    spriteRender = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
    spriteRender->transform()->zOrder(RenderOrders::EnemyOrder);
    spriteRender->setSprite(globalAssets.gameSprites->GetSprite("enemy-low"));
    spriteRender->setSize(spriteRender->getSize() / 10);

    spriteRender->transform()->setParent(transform(), false);
    spriteRender->transform()->angle(180);

    renderAlertSignal = Primitive::CreateEmptyGameObject({Vec2::down / 4.4f})->AddComponent<SpriteRenderer>();
    renderAlertSignal->setSprite(globalAssets.gameSprites->GetSprite("alert-enemy"));
    renderAlertSignal->transform()->setParent(spriteRender->transform(), false);
    renderAlertSignal->setColor(Color::red);
    renderAlertSignal->setSize(Vec2::half);
    renderAlertSignal->gameObject()->SetActive(false);

    // Set Collision Size
    if(spriteRender->getSprite())
        transform()->AddComponent<Collision>()->collideSize = Vec2::Scale(spriteRender->getSprite()->size(), spriteRender->getSize());
    ParticleSystemRef tracePart = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();
    tracePart->transform()->setParent(transform());
    tracePart->transform()->localAngle(0);
    tracePart->setSize(Vec2::one * .1f);
    tracePart->worldSpace = false;
    tracePart->setSource(globalAssets.gameSprites->GetSprite("alert-enemy"));
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
        putEnemyParticleExplode(transform()->position());
        Destroy(gameObject(), after); // can destroy
    }
}

float EKamikadze::getStopDistance() const
{
    return enemy_class_info.kamikadze.stopOnDistance;
}

void EKamikadze::OnUpdate()
{
    float distance;
    Ref<Player> player = WGame::current->player;
    targetTo = player->transform()->position();

    distance = 2;
    renderAlertSignal->transform()->angle(renderAlertSignal->transform()->angle() + 1);

    transform()->LookAtLerp(targetTo, Time::deltaTime() * enemy_class_info.kamikadze.rotateSpeed);

    transform()->Translate(transform()->forward() * Time::deltaTime() * enemy_class_info.kamikadze.speed);
    // transform()->position(Vec2::MoveTowards(transform()->position(), targetTo, Time::deltaTime() * enemy_class_info.kamikadze.speed));

    // Animate size on walk

    // TODO: State walk ?

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
