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

enum WaveState
{
    Standby,
    Delay,
    Active
};

struct WaveInfo
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
    int _roundEnemies;

    float _wavePersentage;
    float waveTimeOut;
    float waveInterval;

    void _killSession();


public:
    EhAlgorithm algorithm;
    EhRank rank;

    Enhancer() = default;

    // Приостанавливает текущую активность системы волн.
    void suspend();

    // Восстанавливает текущую активность волн.
    void resume();

    // Генерирует волну врагов на основе заданных данных.
    // numEnemies - общее количество врагов,
    // numWaves - ожидаемое количество волн.
    // startPercentage - процент врагов, которые будут активны в первой волне (по умолчанию 10%).
    // waveInterval - интервал между волнами в секундах (по умолчанию 5 секунд).
    // Возвращает true, если волна успешно сгенерирована.
    bool generateWave(int numEnemies, int numWaves, float startPercentage = 0.1f, float waveInterval = 5.f);

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

    // Добавляет объект с заданным идентификатором в систему волн.
    void putObject(int id);

};

#endif
