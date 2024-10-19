#include <functional>

#include "SpaceExtractor.hpp"

#include "LevelEnhancer.hpp"

class EhTracker : public Behaviour
{
public:
    int ID;

    void OnDestroy()
    {
        WGame::current->enhancer.putObject( ID );
    }
};

bool Enhancer::generateWave(int numEnemies, int numWaves, float startPercentage, float waveInterval)
{
    if(numEnemies < 0)
        numEnemies = -1;
    else if(numWaves < 0)
        numWaves = -1;

    waveInterval = 0;
    _peekEnemies = 0;

    _numWaves = numWaves;
    _wavePersentage = Math::Clamp(startPercentage, 0.01f, 1.0f);
    waveTimeOut = waveInterval;
    _numEnemies = numEnemies;

    return true;
}

WaveState Enhancer::state()
{
    WaveState _state;

    if(!_numWaves || _numWaves == _activeWave)
        _state = WaveState::Standby;
    else if(waveTimeOut != 0.0f && waveInterval != 0.0f)
        _state = WaveState::Delay;
    else
        _state = WaveState::Active;

    return _state;
}

void Enhancer::doWave()
{
    WaveInfo stat;
    Vec2 s0, s1;
    float off, offset_up;

    switch(state())
    {
        case WaveState::Active:
            if(WGame::current->activeEnemies.empty() && _roundEnemies == _peekEnemies)
            {
                // Set new level instructions
                stat = peekWaveInfo();
                _activeWave = stat.wave;
                _peekEnemies = stat.enemies;

                // Init Next Wave
                waveInterval = Time::time() + waveTimeOut;
                _roundEnemies = 0;

            }
            break;
        case WaveState::Delay:
            // Wait timeout
            if(awaitNextWave() == 0.0f)
            {
                waveInterval = 0; // Invoke
                break;
            }
        case WaveState::Standby:
        default:
            return;
    }

    // Makeable after checking
    if(_roundEnemies < _peekEnemies)
    {
        offset_up = 1.1f;
        s0 = Camera::ViewportToWorldPoint({0.03f, offset_up});
        s1 = Camera::ViewportToWorldPoint({1 - 0.03f, offset_up});
        s1.x -= s0.x;
        s1.y = s0.y;
        off = (s1.x - s0.x) / (_numEnemies * _wavePersentage);
        stat = getWaveInfo();

        int makeCount = Math::Min<int>(_numEnemies - stat.enemies, Math::Ceil(_peekEnemies));

        // Make Clone
        for(int x = 0; x < makeCount; ++x)
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
            // Glud ID
            kamikadze->gameObject()->AddComponent<EhTracker>()->ID = _activatedEnemies + x;
        }

        _roundEnemies += makeCount;
        _activatedEnemies += makeCount;
    }
}

int Enhancer::getWaves()
{
    return _numWaves;
}

WaveInfo Enhancer::getWaveInfo()
{
    WaveInfo stat;

    stat.enemies = WGame::current->activeEnemies.size();
    stat.wave = _activeWave;

    return stat;
}

WaveInfo Enhancer::peekWaveInfo()
{
    WaveInfo stat;

    stat.enemies = Math::Max<int>(1, Math::Ceil(_numEnemies * _wavePersentage));
    stat.wave = Math::Min(_numWaves, _activeWave + 1);

    return stat;
}

float Enhancer::awaitNextWave()
{
    if(waveInterval == 0.0f)
        return -1;

    float t = Math::Max(0.0f, waveInterval - Time::time());

    return t;
}

bool Enhancer::isActive()
{
    return state() == WaveState::Active;
}

void Enhancer::putObject(int id)
{
   // std::cout << "Object " << id << " defeat." << std::endl;
}
