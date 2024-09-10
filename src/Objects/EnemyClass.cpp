#include "EnemyClass.hpp"
#include "Worlds/WGame.hpp"

using namespace RoninEngine::Runtime;

static Sprite *sprite_explode = nullptr;
static Sprite *sprite_explode_flow = nullptr;
static Sprite *sprite_drop_drains = nullptr;
static Sprite *sprite_drop_drains2 = nullptr;

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
    ParticleSystem *particleRef;

    void OnStart() override
    {
        start = Time::time() + 2;
        particleRef->rotatePerFrame = 0;
    }

    void OnUpdate() override
    {
        if(Time::time() < start)
            return;

        Vec2 to;
        to = WGame::current->player->transform()->position();
        particleRef->rotatePerFrame = 120;
        if(transform()->position() == to)
        {
            if(!World::GetCurrentWorld()->StateObjectDestruction(this->gameObject()))
                this->gameObject()->Destroy(0.3f);
            return;
        }
        transform()->position(Vec2::MoveTowards(transform()->position(), to, Time::deltaTime() * 5));
    }
};

ParticleSystem *putEnemyParticleExplode(Vec2 position)
{

    if(sprite_explode == nullptr)
        sprite_explode = assets.gameSprites->GetSprite("explode-v1");
    if(sprite_explode_flow == nullptr)
        sprite_explode_flow = assets.gameSprites->GetSprite("alert-place-flow");
    if(sprite_drop_drains == nullptr)
        sprite_drop_drains = Primitive::CreateSpriteTriangle(false);
    if(sprite_drop_drains2 == nullptr)
        sprite_drop_drains2 = Primitive::CreateSpriteRectangle(false);

    // PARTICLE FIRE
    ParticleSystem *particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->speed = 2;
    particle->direction = Vec2::zero;
    particle->rotatePerFrame = 0;
    particle->worldSpace = false;
    particle->setSource(sprite_explode);
    particle->setLimit(1);
    particle->setInterpolates(10);
    particle->setColors({Color::white, 100}, Color::white, {Color::white, 0});
    particle->setSize(Vec2::half / 4);
    particle->AddComponent<ParticleMoveOut>()->particleRef = particle;
    particle->transform()->zOrder(RenderOrders::ParticlesOrder);

    // PARTICLE DROPS DRAINS
    particle = Primitive::CreateEmptyGameObject(position)->AddComponent<ParticleSystem>();
    particle->loop = false;
    particle->randomDirection = true;
    particle->speed = 0.2f;
    particle->interval = 0.5f;
    particle->startWith = 3;
    particle->setLimit(3);
    particle->setSources(assets.artefacts->GetAtlasObject()->GetSprites());
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
    particle->setSource(sprite_drop_drains);
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
    particle->setSource(sprite_explode_flow);
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
    SpriteRenderer *spriteRender;

    startPoint = transform()->position();
    transform()->layer(GameLayers::EnemyClass);
    transform()->zOrder(RenderOrders::EnemyOrder);

    spriteRender = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
    spriteRender->transform()->zOrder(RenderOrders::EnemyOrder);
    spriteRender->setSprite(assets.gameSprites->GetSprite("enemy-low"));
    spriteRender->setSize(spriteRender->getSize() / 10);

    spriteRender->transform()->setParent(transform(), false);
    spriteRender->transform()->angle(180);

    renderAlertSignal = Primitive::CreateEmptyGameObject({Vec2::down / 4.4f})->AddComponent<SpriteRenderer>();
    renderAlertSignal->setSprite(assets.gameSprites->GetSprite("alert-enemy"));
    renderAlertSignal->transform()->setParent(spriteRender->transform(), false);
    renderAlertSignal->setColor(Color::red);
    renderAlertSignal->setSize(Vec2::half);
    renderAlertSignal->gameObject()->SetActive(false);

    // Set Collision Size
    transform()->AddComponent<Collision>()->collideSize = Vec2::Scale(spriteRender->getSprite()->size(), spriteRender->getSize());
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
    Player *player = WGame::current->player;
    targetTo = player->transform()->position();

    distance = 2;
    renderAlertSignal->transform()->angle(renderAlertSignal->transform()->angle() + 1);

    transform()->LookAtLerp(targetTo, Time::deltaTime() * enemy_class_info.kamikadze.rotateSpeed);

    transform()->Translate(transform()->forward() * Time::deltaTime() * enemy_class_info.kamikadze.speed);
    // transform()->position(Vec2::MoveTowards(transform()->position(), targetTo, Time::deltaTime() * enemy_class_info.kamikadze.speed));

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
