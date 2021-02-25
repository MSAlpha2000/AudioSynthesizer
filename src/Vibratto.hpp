#pragma once
/*
   Эффект 'ВИБРАТТО'
   Два режима: 'ТРЕМОЛО' и 'БЕНД'
   По сути является осцилятором-модулятором (амплитудным (ТРЕМОЛО) или частотным (БЕНД))
*/
class Vibratto {
  public:
    Vibratto(bool type) {
      _type = type;
      _osc = new Oscil;
    }
    ~Vibratto() {
      delete _osc;
    }
    void waveType(uint8_t value) {                                                  //_
      _osc -> type(value);                                                          // |
    }                                                                               // |
    void waveFreq(uint16_t value) {                                                 // |
      _osc -> freq(value);                                                          // |
    }                                                                               // |
    void waveAmpl(uint16_t value) {                                                 //  > Установка параметров волны-модулятора
      _osc -> ampl(value);                                                          // |
    }                                                                               // |
    void wavePhase(uint16_t value, bool direct = false) {                           // |
      _osc -> phase(value, direct);                                                 // |
    }                                                                               //_|
    int16_t get() {                                                                 //Получение модулирующего сигнала
      if (_type) {
        map(_osc -> get(), 0, 4095, -20000, 20000);
      } else {
        map(_osc -> get(), 0, 4095, -2048, 2048);
      }
    }
  private:
    bool _type = false;
    Oscil* _osc;
};
