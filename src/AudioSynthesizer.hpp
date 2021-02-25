/*
                                                АУДИО СИНТЕЗАТОР ДЛЯ ARDUINO DUE

                                                          Функционал:
                        (ДЛЯ УДОБСТВА ПРИВЕДЁН НА ДВУХ ЯЗЫКАХ (ОФИЦИАЛЬНЫЕ АНГЛИЙСКИЕ НАЗВАНИЯ И РУССКИЙ ПЕРЕВОД))
    __________________________________________________________ _______________________________________________________________________
   |  > FAST ANALOG INPUT / OUTPUT                            | > Быстрый аналоговый ввод / вывод                                     |
   |  > STEREO / MONO AUDIO OUTPUT                            | > Стерео / моно аудио выход                                           |
   |  > MONO CHANNEL SELECTION                                | > Выбор моно канала                                                   |
   |  > INTERNAL / EXTERNAL AUDIO PROCESSOR                   | > Внутренний / внешний аудио процессор                                |
   |  > MEMORY FOR NOTES WITH MULTIPLE CARRIER WAVES          | > Память на ноты с множественными несущими волнами                    |
   |  > OSCILLATOR                                            | > Осциллятор (генератор волн)                                         |
   |  > ENVELOPE (AMPLITUDE / FREQUENCY)                      | > Огибающая (амплитудная / частотная)                                 |
   |  > VIBRATTO (TREMOLO (AMPLITUDE) / BEND (FREQUENCY))     | > Вибратто (тремоло (амплитудное) / бенд (частотное))                 |
   |  > LEGATO                                                | > Легато                                                              |
   |  > STAKATTO                                              | > Стакатто                                                            |
   |  > PITCH SHIFTER                                         | > Питч шифтер (изменение тона ноты)                                   |
   |  > SOUND EFFECTS:                                        | > Звуковые эффекты:                                                   |
   |  > > CLIPPING (HARD (DISTORTION) / SOFT (OVERLOAD))      | > > Отсекание (клиппинг) (жёсткий (искажение) / мягкий (перегрузка))  |
   |  > > DELAY                                               | > > Задержка                                                          |
   |  > > CHORUS                                              | > > Хор                                                               |
   |  > > FLANGER                                             | > > Флэнжер                                                           |
   |  > > PHASER                                              | > > Фазовращатель (фейзер)                                            |
   |__________________________________________________________|_______________________________________________________________________|

   Основной класс
   Предоставляет пользователю весь необходимый функционал
*/
#pragma once
//КОНСТАНТЫ
#define A0 7
#define A1 6
#define A2 5
#define A3 4
#define A4 3
#define A5 2
#define A6 1
#define A7 0
#define A8 10
#define A9 11
#define A10 12
#define A11 12
#define D20 8
#define D21 9
#define D52 14
#define TEMP 15
#define SIN 0
#define SQ 1
#define TRN 2
#define SAW 3
#define AM false
#define FM true
#define NO_ANALOG_OUTPUT false
#define USE_ANALOG_OUTPUT true
#define MONO false
#define STEREO true
#define LEFT_CHANNEL false  //Контакт DAC0
#define RIGHT_CHANNEL true  //Контакт DAC1
#define NO_ANALOG_INPUT false
#define USE_ANALOG_INPUT true
#define INTERNAL_SOURCE 0
#define EXTERNAL_SOURCE 4095
#define DEFAULT_SOURCE 2048
//Подключаем библиотеку для работы с прерыванияями по таймеру
#include <DueTimer.h>
//Подключаем все заголовочные файлы для работы с внешними классами
#include "ADCInterface.hpp"
#include "DACInterface.hpp"
#include "Oscillator.hpp"
#include "Envelope.hpp"
#include "Clipping.hpp"
#include "Buffer.hpp"
#include "Delay.hpp"
#include "Chorus.hpp"
#include "Flanger.hpp"
#include "Phaser.hpp"
#include "Vibratto.hpp"
//Псевдонимы функций для прерываний (объявление)
void processAlias(void);
void outputAlias(void);
//Основной класс
class AudioSynthesizerClass {
  private:
    uint8_t _noteCount = 0;
    uint8_t _carrierWaveCount = 0;
    bool _useDAC = true;
    bool _useADC = false;
    Oscil* _mainOsc;
    Oscil* _pitchOsc;
    Envelope* _aModEnv;
    Envelope* _fModEnv;
    bool _useAModEnv = false;
    bool _useFModEnv = false;
    bool _lastUseAModEnv = false;
    bool _lastUseFModEnv = false;
    uint16_t _lastReleaseAModEnv = 0;
    uint16_t _lastReleaseFModEnv = 0;
    Vibratto* _tremolo;
    Vibratto* _bend;
    bool _useTremolo = false;
    bool _useBend = false;
    Delay* _delay;
    Chorus* _chorus;
    Flanger* _flanger;
    Phaser* _phaser;
    uint16_t _phaserFreq = 0;
    int16_t _pitchShift = 0;
    bool _usePitchShift = false;
    bool _legato = false;
    bool _stakatto = false;
    uint8_t _curNoteChannel = 0;
    bool _process = false;
    bool _audioOutputMode = MONO;
    bool _audioOutputChannel = LEFT_CHANNEL;
    uint16_t _intValue = 2048;
    uint16_t _extValue = 2048;
    float _intLevel = 0.50;
    float _extLevel = 0.50;
    uint32_t _timeLeft = 0;
  public:
    void process() {                                                                                                                                                  //Обработка сигнала
      Timer0.stop();                                                                                                                                                      //Остановим таймер на время выполнения функции
      //Устанавливаем начальные значения 2048 (условный 0)
      int16_t val = 2048;         //С этим значением будем работать
      int16_t pitchVal = 2048;    //Это питч
      //Далее работаем только с текущей нотой!
      for (uint8_t i = 0; i < _carrierWaveCount; i++) {                                                                                                                   //Перебираем все несущие волны
        if (_useAModEnv)                                                                                                                                                  //Если используем амплитудную модуляцию огибающей
          _mainOsc[_curNoteChannel * _carrierWaveCount + i].ampl(_mainOsc[_curNoteChannel * _carrierWaveCount + i].getAmpl() + _aModEnv -> get());                        //Амплитуда текущей волны осцилятора (текущ. нота * кол-во несущих волн в одной ноте + текущ. волна) = амплитуде текущей волны + значение амплитудной огибающей
        if (_useFModEnv)                                                                                                                                                  //Если используем частотную модуляцию огибающей
          _mainOsc[_curNoteChannel * _carrierWaveCount + i].freq(_mainOsc[_curNoteChannel * _carrierWaveCount + i].getFreq() + _fModEnv -> get());                        //Частота текущей волны осцилятора (текущ. нота * кол-во несущих волн в одной ноте + текущ. волна) = частоте текущей волны + значение частотной огибающей
        if (_useTremolo)                                                                                                                                                  //Если используем тремоло
          _mainOsc[_curNoteChannel * _carrierWaveCount + i].ampl(_mainOsc[_curNoteChannel * _carrierWaveCount + i].getAmpl() + _tremolo -> get());                        //Амплитуда текущей волны осцилятора (текущ. нота * кол-во несущих волн в одной ноте + текущ. волна) = амплитуде текущей волны + тремоло
        if (_useBend)                                                                                                                                                     //Если используем бенд
          _mainOsc[_curNoteChannel * _carrierWaveCount + i].freq(_mainOsc[_curNoteChannel * _carrierWaveCount + i].getFreq() + _bend -> get());                           //Частота текущей волны осцилятора (текущ. нота * кол-во несущих волн в одной ноте + текущ. волна) = частоте текущей волны + бенд
        val += map(_mainOsc[_curNoteChannel * _carrierWaveCount + i].get(), 0, 4095, -ceil(4095 / (_carrierWaveCount * 2)), ceil(4095 / (_carrierWaveCount * 2)));        //Получаем значение сигнала как текущее значение + значение текущей волны; нормализуем значение текущей волны по количеству несущих волн в одной ноте
        if (_usePitchShift) {                                                                                                                                             //Если используем питч шифтер
          _pitchOsc[i].freq(_mainOsc[_curNoteChannel * _carrierWaveCount + i].getFreq() + _pitchShift);                                                                   //Частота волны питч шифтера = частоте текущей несущей волны + значение питч шифтера                                          _
          _pitchOsc[i].ampl(_mainOsc[_curNoteChannel * _carrierWaveCount + i].getAmpl());                                                                                 //Амплитуда волны питч шифтера = амплитуде текущей несущей волны                                                             |      Волна питч шифтера отличается от текущей несущей волны только частотой (сдвигом частоты)!
          _pitchOsc[i].phase(_mainOsc[_curNoteChannel * _carrierWaveCount + i].getPhase(), _mainOsc[_curNoteChannel * _carrierWaveCount + i].getDirect());                //Фаза волны питч шифтера = фазе текущей несущей волны; направление волны питч шифтера = направлению текущей несущей волны  _|      В остальном она имеет такую же форму (дублирует основную волну)!
          pitchVal += map(_pitchOsc[i].get(), 0, 4095, -ceil(4095 / (_carrierWaveCount * 2)), ceil(4095 / (_carrierWaveCount * 2)));                                      //Получаем значение питч шифтера как текущее значение + значение текущей волны ПИТЧ ШИФТЕРА; нормализуем значение текущей волны ПИТЧ ШИФТЕРА по количеству несущих волн в одной ноте
        }
      }
      if (_usePitchShift) {                             //Если используем питч шифтер, то нормализуем его значение так, чтобы оно попадало в промежуток [0, 4095]
        if (pitchVal > 4095)
          pitchVal = 4095;
        if (pitchVal < 0)
          pitchVal = 0;
      }
      //Нормализуем основное значение так, чтобы оно попадало в промежуток [0, 4095]
      if (val > 4095)
        val = 4095;
      if (val < 0)
        val = 0;
      if (_usePitchShift) {                             //Если используем питч шифтер, то выходное значение считаем как условное нулевое значение (начальное значение переменной 2048) + 0.5 * основное значение + 0.5 значение питч шифтера
        _intValue += map(val, 0, 4095, -1024, 1024);
        _intValue += map(pitchVal, 0, 4095, -1024, 1024);
        if (_intValue > 4095)                               //Выходное значение не должно быть больше 4095!
          _intValue = 4095;
      } else {                                          //Если не используем питч шифтер
        _intValue = val;                                    //Выходное значение = основному значению (т. к. без питч шифтера значения приводить и нормализовывать не надо)
      }
      //Далее работаем только с вых. значением
      //Обрабатываем значение эффектами
      _intValue = Clipping.get(_intValue);                      //Клиппинг - статический класс
      _intValue = _delay -> get(_intValue);
      _intValue = _chorus -> get(_intValue);
      _intValue = _flanger -> get(_intValue);
      uint16_t minFreq = 20000;                         //Мин. частота = макс. возможной, чтобы было куда уменьшать
      uint16_t maxFreq = 0;                             //Макс. частота = мин. возможной, чтобы было куда увеличивать
      for (uint8_t i = 0; i < _carrierWaveCount; i++) { //Считаем мин. и макс. частоту среди всех несущих волн в текущей ноте (если частота < мин., мин. = этой частоте; если частота > макс., макс. = этой частоте)
        minFreq = min(minFreq, _mainOsc[_curNoteChannel * _carrierWaveCount + i].getFreq());
        maxFreq = min(maxFreq, _mainOsc[_curNoteChannel * _carrierWaveCount + i].getFreq());
      }
      _intValue = _phaser -> get(_intValue, map(_phaserFreq, 0, 4095, minFreq, maxFreq));                     //Обрабатываем значение фейзером с частотой, приведённой к промежутку [мин. частота, макс. частота]
      if (!((_useAModEnv and _aModEnv -> isPlaying()) or (_useFModEnv and _fModEnv -> isPlaying())))          //Если используем амплитудную модуляцию и она остановлена и / или (в завис. от использования) используем частотную модуляцию и она остановлена
        _process = false;                                                                                     //Выключаем обработку, т. к. нота больше не звучит
      Timer0.start();                                                                                         //Перезапустим таймер
    }
    void output() {                                                                                                   //Вывод звука
      Timer1.stop();                                                                                                  // Остановим таймер на время выполнения функции
      uint16_t value = _intValue * _intLevel + _extValue * _extLevel;                                                 //Значение = внутр. * внутр. коэффициент + внешн. * внешн. коэффициент
      //Выводим сигнал
      if (_audioOutputMode) {                                                                                         //Если СТЕРЕО режим
        DACInterface.write(value, value);                                                                             //Выводим значение на оба канала
      } else {                                                                                                        //Если МОНО режим
        if (_audioOutputChannel) {                                                                                    //Если ПРАВЫЙ канал
          DACInterface.write(2048, value);                                                                            //На ПРАВЫЙ канал выводим значение, а на ЛЕВЫЙ - условный ноль (2048)
        } else {                                                                                                      //Если ЛЕВЫЙ канал
          DACInterface.write(value);                                                                                  //На ЛЕВЫЙ канал выводим значение (на ПРАВЫЙ - ПО УМОЛЧАНИЮ условный ноль (2048))
        }
      }
      Timer1.start();                                                                                                 //Перезапустим таймер
    }
    void enable(uint32_t sampleRate, uint8_t noteCount, uint8_t carrierWaveCount, bool useDAC = USE_ANALOG_OUTPUT, bool mode = MONO, bool useADC = NO_ANALOG_INPUT, uint16_t src = DEFAULT_SOURCE, bool channel = LEFT_CHANNEL) {            //Разрешить работу синтезатора (частота дискретизациим (в Гц), количество нот, количество несущих волн на одну ноту, использовать ЦАП, использовать АЦП, режим (МОНО / СТЕРЕО), источник (ВНУТРЕННИЙ (INTERNAL_SOURCE) / ВНЕШНИЙ (EXTERNAL_SOURCE) / ПОПОЛАМ (DEFAULT_SOURCE) / <ЗНАЧЕНИЕ от 0 до 4095 (уровень ВНЕШНЕГО источника)>), канал (ЛЕВЫЙ / ПРАВЫЙ)) (класс синтезатора очень требователен к ресурсам, в том числе к памяти и времени выполнения, поэтому пользователь может активировать его только по необходимости (в определённом блоке кода))
      _noteCount = noteCount;                                   //Количество нот (тестироалась на 40 (с патерей качества фейзера), макс. 255!) (для выделения памяти под них)
      _carrierWaveCount = carrierWaveCount;                     //Количество несущих волн на одну ноту (тестировалась 8 (с потерей качества фейзера), макс 255!)
      //Создаём объекты служебных классов и выделяем под них память
      _mainOsc = new Oscil[noteCount * carrierWaveCount];       //Основной осцилятор
      _pitchOsc = new Oscil[carrierWaveCount];                  //Осцилятор для эффекта ПИТЧ ШИФТЕР
      _aModEnv = new Envelope;                                  //Огибающая для амплитудной модуляции
      _fModEnv = new Envelope;                                  //Огибающая для частотной модуляции
      _tremolo = new Vibratto(false);                           //_
      _bend = new Vibratto(true);                               // |
      _delay = new Delay(1000000, sampleRate);                  // |
      _chorus = new Chorus(sampleRate);                         //  > Эффекты
      _flanger = new Flanger(sampleRate);                       // |
      _phaser = new Phaser(sampleRate);                         //_|
      //Устанавливаем флаги
      _useDAC = useDAC;
      _useADC = useADC;
      Timer0.attachInterrupt(processAlias).setFrequency(sampleRate);   //Настраиваем прерывание для обработки сигнала
      if (_useDAC) {                                              //Если используется аналоговый выход
        DACInterface.init();                                      //Инициализация ЦАП
        audioOutputMode(mode, channel);                           //Настроить аудио выход
        source(src);                                              //Установить источник
        Timer1.attachInterrupt(outputAlias).setFrequency(40000);  //Настраиваем прерывание для вывода звука
      }
      if (_useADC)                                                //Если используется аналоговый вход
        ADCInterface.init();                                      //Инициализация АЦП
      Timer0.start();                                             //Запускаем таймер обработки сигнала
      if (useDAC)                                                 //Если используется аналоговый выход
        Timer1.start();                                           //Запускаем таймер вывода звука
    }
    void disable() {            //Запретить работу синтезатора (класс синтезатора очень требователен к ресурсам, в том числе к памяти и времени выполнения, поэтому пользователь может деактивировать его на время, когда он не нужен)
      //Останавливаем таймеры
      Timer0.stop();
      if (_useDAC)                                                 //Если используется аналоговый выход
        Timer1.stop();
      //Отменяем прерывания
      Timer0.detachInterrupt();
      if (_useDAC)                                                 //Если используется аналоговый выход
        Timer1.detachInterrupt();
      //Высвобождаем ресурсы (память)
      delete[] _mainOsc;
      delete[] _pitchOsc;
      delete _aModEnv;
      delete _fModEnv;
      delete _tremolo;
      delete _bend;
      delete _delay;
    }
    void audioOutputMode(bool mode = MONO, bool channel = LEFT_CHANNEL) {                             //Режим аудио выхода (режим (МОНО / СТЕРЕО), канал (ЛЕВЫЙ / ПРАВЫЙ) (для режима МОНО))
      if (_useDAC) {                                                                                  //Настроить аудио выход можем только если используем (инициализировали) ЦАП!
        _audioOutputMode = mode;
        audioOutputChannel(channel);
      }
    }
    void audioOutputChannel(bool channel = LEFT_CHANNEL) {                                            //Канал аудио выхода (ЛЕВЫЙ / ПРАВЫЙ) (для режима МОНО)
      if (_useDAC) {                                                                                  //Настроить канал можем только если используем (инициализировали) ЦАП!
        _audioOutputChannel = channel;
      }
    }
    void waveType(uint8_t noteChannel, uint8_t waveChannel, uint16_t value) {                         //Тип волны (номер ноты, номер волны, значение)
      _mainOsc[noteChannel * _carrierWaveCount + waveChannel].type(value);
    }
    void waveFreq(uint8_t noteChannel, uint8_t waveChannel, uint16_t value) {                         //Частота волны (номер ноты, номер волны, значение)
      _mainOsc[noteChannel * _carrierWaveCount + waveChannel].freq(value);
    }
    void waveAmpl(uint8_t noteChannel, uint8_t waveChannel, uint16_t value) {                         //Амплитуда волны (номер ноты, номер волны, значение)
      _mainOsc[noteChannel * _carrierWaveCount + waveChannel].ampl(value);
    }
    void wavePhase(uint8_t noteChannel, uint8_t waveChannel, uint16_t value, bool direct = true) {    //Фаза волны (номер ноты, номер волны, значение, направление)
      _mainOsc[noteChannel * _carrierWaveCount + waveChannel].phase(value, direct);
    }
    void envelope(uint16_t minThr, uint16_t maxThr, uint16_t attackTime, uint16_t holdTime, uint16_t decayTime, int16_t sustainVal, uint16_t releaseTime, bool modulation = AM) {            //Огибающая (мин. порог, макс. порог, время атаки, время удержания, время спада, уровень поддержания, время затухания, модуляция (АМПЛИТУДНАЯ (по умолчанию) / ЧАСТОТНАЯ))
      if (modulation) {
        if (minThr < -20000)
          minThr = -20000;
        _fModEnv -> minThreshold(minThr);
        if (maxThr > 20000)
          minThr = 20000;
        _fModEnv -> maxThreshold(maxThr);
        _fModEnv -> attack(attackTime);
        _fModEnv -> hold(holdTime);
        _fModEnv -> decay(decayTime);
        _fModEnv -> sustain(sustainVal);
        _fModEnv -> release(releaseTime);
      } else {
        if (minThr < -2048)
          minThr = -2048;
        _aModEnv -> minThreshold(minThr);
        if (maxThr > 2048)
          minThr = 2048;
        _aModEnv -> maxThreshold(maxThr);
        _aModEnv -> attack(attackTime);
        _aModEnv -> hold(holdTime);
        _aModEnv -> decay(decayTime);
        _aModEnv -> sustain(sustainVal);
        _aModEnv -> release(releaseTime);
      }
    }
    void useEnvelope(bool modulation = AM, bool value = true) {                     //Использовать огибающую (модуляция, значение)
      if (modulation)
        _useFModEnv = value;
      else
        _useAModEnv = value;
    }
    void clipping(uint16_t thr, uint16_t smooth) {                                  //Клиппинг (пороu, плавность)
      Clipping.threshold(thr);
      Clipping.smoothness(smooth);
    }
    void tremolo(uint8_t waveTypeVal, uint16_t waveFreqVal, uint16_t waveAmplVal, uint16_t wavePhaseVal, bool waveDirectVal = true) {                     //Тремоло (тип волны, частота волны, амплитуда волны, фаза волны, направление волны)
      _tremolo -> waveType(waveTypeVal);
      if (waveFreqVal > 10)                               //Частота не должна превышать 10 Гц
        waveFreqVal = 10;
      _tremolo -> waveFreq(waveFreqVal);
      _tremolo -> waveAmpl(waveAmplVal);
      _tremolo -> wavePhase(wavePhaseVal, waveDirectVal);
    }
    void useTremolo(bool value) {
      _useTremolo = value;
    }
    void bend(uint8_t waveTypeVal, uint16_t waveFreqVal, uint16_t waveAmplVal, uint16_t wavePhaseVal, bool waveDirectVal = true) {                        //Бенд (тип волны, частота волны, амплитуда волны, фаза волны, направление волны)
      _bend -> waveType(waveTypeVal);
      if (waveFreqVal > 10)                               //Частота не должна превышать 10 Гц
        waveFreqVal = 10;
      _bend -> waveFreq(waveFreqVal);
      _bend -> waveAmpl(waveAmplVal);
      _bend -> wavePhase(wavePhaseVal, waveDirectVal);
    }
    void useBend(bool value) {
      _useBend = value;
    }
    void delay(uint32_t delayTime, uint16_t balanceVal, uint16_t feedbackLevel) {                                             //Задержка
      _delay -> delay(delayTime);
      _delay -> balance(balanceVal);
      _delay -> feedback(feedbackLevel);
    }
    void chorus(uint16_t balanceVal, uint16_t feedbackLevel) {                                                                //Хор
      _chorus -> balance(balanceVal);
      _chorus -> feedback(feedbackLevel);
    }
    void chorusWave(uint8_t waveType, uint16_t waveFreq, uint16_t waveAmpl, uint16_t wavePhase, bool waveDirect = true) {     //Волна хора
      _chorus -> wave(waveType, waveFreq, waveAmpl, wavePhase, waveDirect);
    }
    void flanger(uint16_t balanceVal, uint16_t feedbackLevel) {                                                               //Флэнжер
      _flanger -> balance(balanceVal);
      _flanger -> feedback(feedbackLevel);
    }
    void flangerWave(uint8_t waveType, uint16_t waveFreq, uint16_t waveAmpl, uint16_t wavePhase, bool waveDirect = true) {    //Волна Флэнжера
      _flanger -> wave(waveType, waveFreq, waveAmpl, wavePhase, waveDirect);
    }
    void phaser(uint16_t balanceVal, uint16_t feedbackLevel) {                                                                //Фейзер
      _phaser -> balance(balanceVal);
      _phaser -> feedback(feedbackLevel);
    }
    void phaserWave(uint8_t waveType, uint16_t waveFreq, uint16_t waveAmpl, uint16_t wavePhase, bool waveDirect = true) {     //Волна фейзера
      _phaser -> wave(waveType, waveFreq, waveAmpl, wavePhase, waveDirect);
    }
    void phaserFreq(uint16_t value) {                                                                                         //Частота фейзера
      if (value > 4095)
        value = 4095;
      _phaserFreq = value;
    }
    void pitchShifter(int16_t value) {                                                                                        //Питч шифтер
      if (value >= -20000 and value <= 20000)                                                                                 //Значение должно быть в промежутке [-20000, 20000] (от -20 кГц до 20 кГц)
        _pitchShift = value;
    }
    void usePitchShifter(bool value = true) {                                                                                 //Использовать питч шифтер
      _usePitchShift = value;
    }
    void legato(bool value = true) {                                                                                          //Легато (плавный переход между нотами)
      if (value != _legato) {                                                                                                 //Если новое значение не равно текущему значению легато
        _legato = value;                                                                                                      //Сохраним новое значение
        if (_legato) {                                                                                                        //Если легато включено
          //Сохраним предыдущее состояние огибающих
          _lastUseAModEnv = _useAModEnv;
          _lastUseFModEnv = _useFModEnv;
          //Если огибающие выключены, включим их
          if (!_useAModEnv)
            _useAModEnv = true;
          if (!_useFModEnv)
            _useFModEnv = true;
          //Сохраним предыдущее временя затухания огибающих
          _lastReleaseAModEnv = _aModEnv -> getRelease();
          _lastReleaseFModEnv = _fModEnv -> getRelease();
          //Установим временя затухания огибающих на максимум (1 с)
          _aModEnv -> release(1000);
          _fModEnv -> release(1000);
        } else {                                                                                                              //Если легато выключено
          //Вернём предыдущие значения огибающих
          _useAModEnv = _lastUseAModEnv;
          _useFModEnv = _lastUseFModEnv;
          _aModEnv -> release(_lastReleaseAModEnv);
          _fModEnv -> release(_lastReleaseFModEnv);
        }
      }
    }
    void stakatto(bool value = true) {                                                                                        //Стакатто (отрывистое исполнение нот)
      if (value != _stakatto) {                                                                                               //Если новое значение не равно текущему значению стакатто
        _stakatto = value;                                                                                                    //Сохраним новое значение
        if (_stakatto) {                                                                                                      //Если стакатто включено
          //Сохраним предыдущее состояние огибающих
          _lastUseAModEnv = _useAModEnv;
          _lastUseFModEnv = _useFModEnv;
          //Если огибающие включены, выключим их
          if (_useAModEnv)
            _useAModEnv = false;
          if (_useFModEnv)
            _useFModEnv = false;
        } else {                                                                                                              //Если стакатто выключено
          //Вернём предыдущие состояние огибающих
          _useAModEnv = _lastUseAModEnv;
          _useFModEnv = _lastUseFModEnv;
        }
      }
    }
    void note(uint8_t channel) {                     //Играть ноту (номер)
      if (channel >= _noteCount)                     //Номер ноты должен быть меньше количества нот! (нумерация начинается с 0!)
        return;
      //Запускаем огибающие, если они включены
      if (_useAModEnv)
        _aModEnv -> note();
      if (_useFModEnv)
        _fModEnv -> note();
      _curNoteChannel = channel;                    //Устанавливаем номер текущей ноты
      _process = true;                              //Включаем обработку, т. к. начинается звучание ноты
    }
    void note() {                                   //"Отпустить" ноту (без параметра (void))
      //Останавливаем огибающие, если они включены
      if (_useAModEnv)
        _aModEnv -> note(false);
      if (_useFModEnv)
        _fModEnv -> note(false);
    }
    uint16_t internal() {                             //Получить значение с ВНУТРЕННЕГО процессора
      return _intValue;
    }
    void external(uint16_t value) {                   //Задать значение с ВНЕШНЕГО процессора
      if (_useDAC) {                                  //Задавать значение с ВНЕШНЕГО процессора можем только если используем (инициализировали) ЦАП!
        if (value > 4095)                             //Значение не должно превышать 4095!
          value = 4095;
        _extValue = value;
      }
    }
    void source(uint16_t source = DEFAULT_SOURCE) {   //Установить источник (ВНУТРЕННИЙ (INTERNAL_SOURCE) / ВНЕШНИЙ (EXTERNAL_SOURCE) / ПОПОЛАМ (DEFAULT_SOURCE) / <ЗНАЧЕНИЕ от 0 до 4095 (уровень ВНЕШНЕГО источника)>)
      if (_useDAC) {                                  //Установливать источник можем только если используем (инициализировали) ЦАП!
        //Считаем коэффициенты
        _intLevel = map(source, 0, 4095, 1.00, 0.00);
        _extLevel = map(source, 0, 4095, 0.00, 1.00);
      }
    }
    uint16_t input(uint8_t channel) {       //Аналоговый ввод (канал)
      if (_useADC)                          //Можем использовать эту функцию только если используем (инициализировали) АЦП!
        return ADCInterface.read(channel);
      else
        return NULL;
    }
    bool getProcess() {                     //Идёт ли обработка сигнала
      return _process;
    }
};
static AudioSynthesizerClass AudioSynthesizer;  //Статический класс, т. к. одновременно можно использовать только ОДИН синтезатор (его можно считать интерфейсом (аппаратным / програмным))
//Псевдонимы функций для прерываний (определение)
void processAlias(void) {
  AudioSynthesizer.process();
}
void outputAlias(void) {
  AudioSynthesizer.output();
}
