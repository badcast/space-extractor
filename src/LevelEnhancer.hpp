#ifndef _LEVEL_ENHANCER_HPP_
#define _LEVEL_ENHANCER_HPP_

// Алгоритмы для LevelEnhancer -
enum EhAlgorithm
{
    SequentialEnhancer,  // - Последовательный улучшитель, который ожидает завершения других алгоритмов перед активацией.
    ConcurrentGenerator, // - Параллельный генератор, который создает алгоритмы в реальном времени, независимо от других.
    SynchronizedBooster  // - Синхронизированный усилитель, который согласовывает свою активацию с другими алгоритмами.
};

enum EhRank
{
    Low,
    Middle,
    Hard
};

enum EhState
{
    Standby,
    Delay,
    Active
};

struct EhStat
{
    int enemies;
    int wave;
};

class Enhancer
{
private:
    int _numWaves;
    int _numEnemies;

    int _activeWave;
    int _peekEnemies;
    int _activatedEnemies;

    float _wavePersentage;
    float _waveInterval;
    float _intervals;

    std::vector<int> _evils;

    void _killSession();


public:
    EhAlgorithm algorithm;
    EhRank rank;

    Enhancer() = default;

    // Отправить запрос "Приостановить"
    void suspend();

    // Генерирует уровень волны, на основе данных
    // Количество врагов, и ожидаемое количество волн.
    // Важно, чтобы количество врагов было больше или равно (>=) на число ожидаемых волн (!)
    bool generateWave(int numEnemies, int numWaves, float startPercentage = 0.1f, float waveInterval = 5.f);

    void doWave();

    int getWaves();

    EhStat activeWaveInfo();

    EhStat peekWaveInfo();

    float after();

    EhState state();

    bool isActive();

    void putObject(int id);
};

#endif
