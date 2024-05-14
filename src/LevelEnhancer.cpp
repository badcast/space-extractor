#include "SpaceExtractor.hpp"

#include "LevelEnhancer.hpp"

class EhTracker : public Behaviour
{
public:
    int ID;

    void OnDestroy()
    {
        WGame::current->enhancer.putObject(ID);
    }
};

bool Enhancer::generateWave(int numEnemies, int numWaves, float startPercentage, float waveInterval)
{
    if(numEnemies < 0)
        numEnemies = -1;
    else if(numWaves < 0)
        numWaves = -1;

    _intervals = 0;
    _peekEnemies = 0;

    _numWaves = numWaves;
    _wavePersentage = Math::Clamp(startPercentage, 0.01f, 1.0f);
    _waveInterval = waveInterval;
    _numEnemies = numEnemies;

    return true;
}

EhState Enhancer::state()
{
    EhState _state;

    if(_evils.empty() && (!_numWaves || _numWaves == _activeWave))
        _state = EhState::Standby;
    else if(_waveInterval != 0.0f && _intervals != 0.0f)
        _state = EhState::Delay;
    else
        _state = EhState::Active;

    return _state;
}

void Enhancer::doWave()
{
    EhState _state;

    _state = state();

    // State is Standby
    if(_state == EhState::Standby)
        return;

    EhStat stat;
    Vec2 s0, s1;
    float off, offset_up;
    int maxCount;

    auto __spawn__ = [&]() -> GameObject *
    {
        Collision *collision;
        EKamikadze *kamikadze;

        kamikadze = Primitive::CreateEmptyGameObject("EKamikadze")->AddComponent<EKamikadze>();
        collision = kamikadze->GetComponent<Collision>();
        collision->targetLayer = static_cast<int>(GameLayers::PlayerOrBullet);
        // On Collision
        kamikadze->AddOnDestroy(
            [](Component *self)
            {
                AudioClip *clip = assets.gameSounds->GetAudioClip("space-explode");
                AudioSource::PlayClipAtPoint(clip, self->transform()->position(), 0.2f);
                WGame::current->activeEnemies.erase(self->GetComponent<Enemy>());
                // if(self->GetComponent<Enemy>()->healthPoint)
                //     putEnemyParticleExplode(self->transform()->position());
            });

        WGame::current->activeEnemies.insert(kamikadze);

        // s0.y = Random::Range(s0.y, s0.y * 2);
        kamikadze->transform()->position(s0);
        kamikadze->gameObject()->SetLayer(static_cast<int>(GameLayers::EnemyClass));
        s0.x += off;
        return kamikadze->gameObject();
    };

    switch(_state)
    {
        case EhState::Active:
            if(WGame::current->activeEnemies.empty() && _activatedEnemies == _peekEnemies)
            {

                if(_peekEnemies)
                    _intervals = Time::time() + _waveInterval;

                // Set new level instructions
                stat = peekWaveInfo();
                _activeWave = stat.wave;
                _peekEnemies = stat.enemies;
            }
            break;
        case EhState::Delay:
            // Wait
            if(after() == 0.0f)
            {
                _intervals = 0; // Invoke
                break;
            }
        default:
            return;
    }

    int cur = activeWaveInfo().enemies;

    // Makeable after checking
    if(cur < _peekEnemies)
    {
        offset_up = -0.05f;
        s0 = Camera::ViewportToWorldPoint({0.03f, offset_up});
        s1 = Camera::ViewportToWorldPoint({1 - 0.03f, offset_up});
        s1.x -= s0.x;
        s1.y = s0.y;
        off = (s1.x - s0.x) / (_numEnemies * _wavePersentage);

        int makeCount = Math::Min<int>(maxCount - cur, Math::Ceil(_peekEnemies));

        for(int x = 0; x < makeCount; ++x)
        {
            GameObject *spawned = __spawn__();

            // Glud ID
            spawned->AddComponent<EhTracker>()->ID = _activatedEnemies + x;
        }
        _activatedEnemies += makeCount;
    }
}

int Enhancer::getWaves()
{
    return _numWaves;
}

EhStat Enhancer::activeWaveInfo()
{
    EhStat stat;

    stat.enemies = WGame::current->activeEnemies.size();
    stat.wave = _activeWave;

    return stat;
}

EhStat Enhancer::peekWaveInfo()
{
    EhStat stat;

    stat.enemies = Math::Max<int>(1, Math::Ceil(_numEnemies * _wavePersentage));
    stat.wave = Math::Min(_numWaves, _activeWave + 1);

    return stat;
}

float Enhancer::after()
{
    if(_intervals == 0.0f)
        return -1;

    float t = Math::Max(0.0f, _intervals - Time::time());

    return t;
}

bool Enhancer::isActive()
{
    return state() == EhState::Active;
}

void Enhancer::putObject(int id)
{
}
