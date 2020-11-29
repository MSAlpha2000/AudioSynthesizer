#pragma once
/*
   Эффет 'ХОР'
   Задержка с модуляцией волной осцилятора
*/
class Chorus {
  public:
    Chorus(uint32_t sampleRate) {
      _delay = new Delay(DELAY, sampleRate);                                                                            //Задержка (стандарт - 30000 (не работает с параметрами: частота дискретизации 40 кГц, 40 нот, 8 несущих волн (не хватает ресурсов (памяти))!), оптимально - 10000)
      _osc = new Oscil;
    }
    ~Chorus() {
      delete _delay;
      delete _osc;
    }
    void balance(uint16_t value) {                                                                                      //Баланс wet/dry (смесь обработанного и необработанного сигнала)
      _delay -> balance(value);
    }
    void feedback(uint16_t value) {                                                                                     //Уровень линнии обратной связи
      _delay -> feedback(value);
    }
    void wave(uint8_t waveType, uint16_t waveFreq, uint16_t waveAmpl, uint16_t wavePhase, bool waveDirect = true) {     //Установка параметров волны модулятора
      _osc -> type(waveType);
      if (waveFreq > 3)                                                                                                 //Максимум 3 Гц
        waveFreq = 3;
      _osc -> freq(waveFreq);
      _osc -> ampl(waveAmpl);
      _osc -> phase(wavePhase, waveDirect);
    }
    uint16_t get(uint16_t input) {                                                                                      //Получить обработанный сигнал
      _delay -> delay(map(_osc -> get(), 0, 4095, 20000, 30000));                                                       //Устанавливаем задержку по волне
      return _delay -> get(input);                                                                                      //Получаем
    }
  private:
    Delay* _delay;
    Oscil* _osc;
};
