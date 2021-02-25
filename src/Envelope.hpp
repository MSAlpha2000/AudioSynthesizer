#pragma once
/* Огибающая
   Для изменения параметров волны (модуляции) во времени
*/
class Envelope {
  public:
    void attack(uint16_t value) {                                         //Атака
      if (value > 1000)
        value = 1000;
      _attack = value;
    }
    void hold(uint16_t value) {                                           //Удержание
      if (value > 1000)
        value = 1000;
      _hold = value;
    }
    void decay(uint16_t value) {                                          //Спад
      if (value > 1000)
        value = 1000;
      _decay = value;
    }
    void sustain(int16_t value) {                                         //Уровень поддержания (поддержка)
      if (value >= _minThreshold and value <= _maxThreshold)              //Должен быть в промежутке от мин. порога до макс. порога
        _sustain = value;
    }
    void release(uint16_t value) {                                        //Затухание
      if (value > 1000)
        value = 1000;
      _release = value;
    }
    void minThreshold(int16_t value) {                                    //Мин. порог
      _minThreshold = value;
      _value = _minThreshold;
      _phase = 0;                                                         //Сбросим фазу, т. к. поменяли промежуток допустимых значений
      _play = false;                                                      //Остановим огибающую, т. к. поменяли промежуток допустимых значений
      if (direct and _minThreshold > _maxThreshold)                       //Если график в стандартном положении и мин. порог > макс. порога, "развернём" (инвертируем) график
        direct = false;
      if (!direct and _minThreshold <= _maxThreshold)                     //Если график в развёрнутом (инвертированном) положении и мин. порог меньше или равен макс. порогу, вернём график в стандартное положение
        direct = true;
    }
    void maxThreshold(int16_t value) {                                    //Макс. порог
      _maxThreshold = value;
      _value = _minThreshold;
      _phase = 0;                                                         //Сбросим фазу, т. к. поменяли промежуток допустимых значений
      _play = false;                                                      //Остановим огибающую, т. к. поменяли промежуток допустимых значений
      if (direct and _minThreshold > _maxThreshold)                       //Если график в стандартном положении и мин. порог > макс. порога, "развернём" (инвертируем) график
        direct = false;
      if (!direct and _minThreshold <= _maxThreshold)                     //Если график в развёрнутом (инвертированном) положении и мин. порог меньше или равен макс. порогу, вернём график в стандартное положение
        direct = true;
    }
    void note(bool value = true) {                                        //Нота
      if (value) {                                                        //Если нота играется
        if (!_play)                                                       //И огибающая остановлена
          _play = true;                                                   //Запустить огибающую
        _phase = 0;                                                       //С начальной фазы (атака)
      } else {                                                            //Иначе (если нота не играется)
        if (_play and _phase != 4)                                        //Если огибающая запущена и находится НЕ в стадии затухания
          _phase = 4;                                                     //Начать стадию затухания
      }
      _timeLeft = millis();                                               //Перезапустить таймер
    }
    int16_t get() {                                                       //Получить значение огибающей (для модуляции)
      if (_play) {                                                        //Если огибающая запущена
        switch (_phase) {                                                 //Действия зависят от фазы огибающей
          case 1:                                                         //Удержание
            if (millis() - _timeLeft >= _hold) {                          //Если прошло время удержания
              _phase++;                                                   //Следующая фаза
              _timeLeft = millis();                                       //Перезапустить таймер
            }
            break;
          case 2:                                                                         //Спад
            if (millis() - _timeLeft >= _decay / abs(_maxThreshold - _sustain)) {         //Если прошло время спада / |разность макс. порга и уровня удержания (след. фазы)|
              if (direct) {                                                               //Если график огибающей в стандартном состоянии
                if (_value > _sustain)                                                    //И значение > уровня удержания
                  _value--;                                                               //Значение = значение - 1 (декремент)
                else                                                                      //Иначе
                  _phase++;                                                               //След. фаза
              } else {                                                                    //Если график огибающей в инвертированном состоянии
                if (_value < _sustain)                                                    //И значение < уровня удержания
                  _value++;                                                               //Значение = значение + 1 (инкремент)
                else                                                                      //Иначе
                  _phase++;                                                               //След. фаза
              }
              _timeLeft = millis();                                                       //Перезапустить таймер
            }
            break;
          case 3:                                                                         //Удержание
            break;                                                                        //Просто ждём остановки игры ноты (note(false))
          case 4:                                                                         //Затухание
            if (millis() - _timeLeft >= _release / abs(_sustain - _minThreshold)) {       //Если прошло время затухания / |разность уровня удержания и мин. порога|
              if (direct) {                                                               //Если график огибающей в стандартном состоянии
                if (_value > _minThreshold) {                                             //И значение > мин. порога
                  _value--;                                                               //Значение = значение - 1 (декремент)
                } else {                                                                  //Иначе
                  _phase = 0;                                                             //Обнулим фазу
                  _play = false;                                                          //Остановим огибающую
                }
              } else {                                                                    //Если график огибающей в инвертированном состоянии
                if (_value < _minThreshold) {                                             //И значение < мин. порога
                  _value++;                                                               //Значение = значение + 1 (инкремент)
                } else {                                                                  //Иначе
                  _phase = 0;                                                             //Обнулим фазу
                  _play = false;                                                          //Остановим огибающую
                }
              }
              _timeLeft = millis();                                                       //Перезапустим таймер
            }
            break;
          default:                                                                        //Атака
            if (millis() - _timeLeft >= _attack / abs(_maxThreshold - _minThreshold)) {   //Если прошло время атаки / |разность макс. порга и мин. порога|
              if (direct) {                                                               //Если график огибающей в стандартном состоянии
                if (_value < _maxThreshold)                                               //И значение < макс. порога
                  _value++;                                                               //Значение = значение + 1 (инкремент)
                else                                                                      //Иначе
                  _phase++;                                                               //След. фаза
              } else {                                                                    //Если график огибающей в инвертированном состоянии
                if (_value > _maxThreshold)                                               //И значение > макс. порога
                  _value--;                                                               //Значение = значение - 1 (декремент)
                else                                                                      //Иначе
                  _phase++;                                                               //След. фаза
              }
              _timeLeft = millis();                                                       //Перезапустим таймер
            }
            break;
        }
      }
      return _value;
    }
    bool isPlaying() {                                                                    //Запущена ли огибающая
      return _play;
    }
    uint16_t getRelease() {                                                               //Находится ли огибающая в фазе затухания
      return _release;
    }
  private:
    uint16_t _attack = 0;
    uint16_t _hold = 0;
    uint16_t _decay = 0;
    int16_t _sustain = 0;
    uint16_t _release = 0;
    int16_t _minThreshold = 0;
    int16_t _maxThreshold = 4095;
    uint16_t _timeLeft = 0;
    uint8_t _phase = 0;
    bool _play = false;
    bool direct = true;
    int16_t _value = 0;
};
