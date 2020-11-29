#pragma once
/*
   Задержка
   И эффект 'ЗАДЕРЖКА', и служебный класс.
*/
class Delay {
  public:
    Delay(uint32_t time, uint32_t sampleRate) {                                 //Макс. время задержки, частота дискретизации
      _time = time;
      _sampleRate = sampleRate;
      _buffer = new Buffer(_time / round(1000000 / _sampleRate));               //Создаём буффер с параметром необходимого кол-ва элементов массива (макс. время / период дискретизации)
    }
    ~Delay() {
      delete _buffer;
    }
    void delay(uint32_t value) {                                                //Установить время текущей задержки
      if (value <= _time)                                                       //Время текущей задержки должно быть меньше максимально возможного
        _delay = value;
    }
    void balance(uint16_t value) {                                              //Баланс wet/dry (смесь обработанного и необработанного сигнала)
      if (value > 4095)
        value = 4095;
      _wet = map(value, 0, 4095, 0.00, 1.00);
      _dry = 1.00 - _wet;
    }
    void feedback(uint16_t value) {                                             //Уровень линнии обратной связи
      if (value > 4095)
        value = 4095;
      _feedback = map(value, 0, 4095, 0.00, 1.00);
    }
    uint16_t get(uint16_t input) {                                              //Получить обработанный сигнал
      uint16_t current = _buffer -> get(_delay / round(1000000 / _sampleRate)); //Текущее значение (получаем из буффера по текущей задержке)
      uint16_t output = _dry * input + _wet * current;                          //Считаем выходной сигнал как смесь необработанного (dry) и обработанного (wet)
      uint16_t value = 2048;
      value += map(input, 0, 4095, -1024, 1024);
      value += map(_feedback * current, 0, 4095, -1024, 1024);                  //Считаем сигнал обратной связи
      if (value > 4095)
        value = 4095;
      _buffer -> put(value);                                                    //Записываем в буффер сигнал обратной связи
      return output;
    }
  private:
    float _wet = 0.50;
    float _dry = 0.50;
    float _feedback = 0.50;
    uint32_t _delay = 0;
    uint32_t _time = 0;
    uint32_t _sampleRate = 0;
    Buffer* _buffer;
};
