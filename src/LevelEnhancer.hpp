#ifndef _LEVEL_ENHANCER_HPP_
#define _LEVEL_ENHANCER_HPP_

#include <Worlds/WGame.hpp>

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

enum WaveState
{
    Finish,
    Delay,
    Active
};

struct WaveInfo
{
    int enemies;
    int maxEnemies;
    int wave;
};

class Enhancer
{
private:
    int _maxWaves;
    int _maxEnemies;

    int _activeWave;
    int _peekEnemies;
    int _activatedEnemies;
    int _roundEnemies;
    int _startEnemyCount;

    float _addPercentage;
    float _waveTimeOut;
    float waveDelayTicks;

    WaveInfo _lastWave;

    std::function<GameObjectRef (Vec2)> _addPoint;

    void _killSession();
public:
    EhAlgorithm algorithm;
    EhRank rank;

    Enhancer() = default;

    void setDelegate(std::function<GameObjectRef(Vec2)> addPoint);

    // Приостанавливает текущую активность системы волн.
    void suspend();

    // Восстанавливает текущую активность волн.
    void resume();

    // Генерирует волну врагов на основе заданных данных.
    // maxEnemies - общее количество врагов,
    // maxWaves - ожидаемое количество волн.
    // startEnemyCount - процент врагов, которые будут активны в первой волне (по умолчанию 1).
    // waveTimeout - интервал между волнами в секундах (по умолчанию 5 секунд).
    // addPercentage - процент добавление врагов при след. волнах (по умполчанию 10%).
    // Возвращает true, если волна успешно сгенерирована.
    bool generateWave(int maxEnemies, int maxWaves, int startEnemyCount = 1, float addPercentage = 0.1f, float waveTimeout = 5.f);

    // Циклический обрабатывает событие волн.
    void doWave();

    // Возвращает общее количество сгенерированных волн.
    int getWaves();

    // Возвращает информацию о текущей волне.
    WaveInfo getWaveInfo();

    // Возвращает информацию о следующей волне без её запуска.
    WaveInfo peekWaveInfo();

    // Возвращает время ожидания для следующей волне.
    float awaitNextWave();

    // Возвращает текущее состояние системы волн.
    WaveState state();

    // Проверяет, активна ли система волн.
    bool isActive();

    // Проверяет, закончился ли система волн.
    bool isFinished();

    // Добавляет объект с заданным идентификатором в систему волн.
    void putObject(int id);

};

#endif
