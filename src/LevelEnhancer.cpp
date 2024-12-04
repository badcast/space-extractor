#include <ronin/Debug.h>

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

void Enhancer::setDelegate(std::function<GameObjectRef(Vec2)> addPoint)
{
    _addPoint = addPoint;
}

bool Enhancer::generateWave(int maxEnemies, int maxWaves, int startEnemyCount, float addPercentage, float waveTimeout)
{
    if(maxEnemies < 0)
        maxEnemies = -1;
    else if(maxWaves < 0)
        maxWaves = -1;

    _peekEnemies = 0;
    _roundEnemies = 0;
    _activeWave = 0;
    _addPercentage = addPercentage;

    _maxWaves = maxWaves;
    _maxEnemies = maxEnemies;
    _waveTimeOut = waveTimeout;
    _startEnemyCount = Math::Clamp<int>(startEnemyCount, 1, maxEnemies);

    return true;
}

WaveState Enhancer::state()
{
    WaveState _state;
    if(!_maxWaves || _maxWaves == _activeWave)
        _state = WaveState::Finish;
    else if(waveDelayTicks != 0.0f)
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
    int x,y;
    GameObjectRef objFrom;

    switch(state())
    {
        case WaveState::Active:

            // Set Begin of
            if(!WGame::current->activeEnemies.empty() || _roundEnemies != _peekEnemies)
                break;

            stat = peekWaveInfo();
            _activeWave = stat.wave;
            _peekEnemies = stat.maxEnemies;

            // Set End of
            if(_roundEnemies)
            {
                waveDelayTicks = Time::time() + _waveTimeOut;
                _roundEnemies = 0;
                return;
            }
            break;
        case WaveState::Delay:
            // Wait timeout
            if(awaitNextWave() == 0.0f)
            {
                // Reset Ticks
                waveDelayTicks = 0;
            }
        case WaveState::Finish:
        default:
            return;
    }

    // Make after checking
    if(_roundEnemies < _peekEnemies)
    {
        stat = getWaveInfo();
        offset_up = 1.1f;
        s0 = Camera::ViewportToWorldPoint({0.03f, offset_up});
        s1 = Camera::ViewportToWorldPoint({1 - 0.03f, offset_up});
        off = (s1.x - s0.x) / (stat.maxEnemies);


        // Make Clones
        y = Math::Min<int>(stat.maxEnemies-_roundEnemies, _peekEnemies);
        for(x = 0; x < y; ++x)
        {
            objFrom = _addPoint(s0);
            objFrom->AddComponent<EhTracker>()->ID = _activatedEnemies + x;
            s0.x += off;
        }

        _roundEnemies += y;
        _activatedEnemies += y;
    }
}

int Enhancer::getWaves()
{
    return _maxWaves;
}

WaveInfo Enhancer::getWaveInfo()
{
    WaveInfo stat;

    stat.enemies = WGame::current->activeEnemies.size();
    stat.maxEnemies = _peekEnemies;
    stat.wave = _activeWave;

    return stat;
}

WaveInfo Enhancer::peekWaveInfo()
{
    WaveInfo stat;

    // Every null
    stat.enemies = 0;
    stat.maxEnemies = Math::Max<int>(1, Math::Min(_maxEnemies, this->_startEnemyCount));
    stat.wave = Math::Min(_maxWaves, _activeWave + 1);

    return stat;
}

float Enhancer::awaitNextWave()
{
    if(waveDelayTicks == 0.0f)
        return -1;

    float t = Math::Max(0.0f, waveDelayTicks - Time::time());

    return t;
}

bool Enhancer::isActive()
{
    return state() == WaveState::Active;
}

bool Enhancer::isFinished()
{
    return state() == WaveState::Finish;
}

void Enhancer::putObject(int id)
{
}
