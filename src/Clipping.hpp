#pragma once
/*
   Эффект 'КЛИППИНГ' (применяется, например, в электрогитаре
   Два редима: ЖЁСТКИЙ (DISTORTION) и МЯГКИЙ (OVERDRIVE)
*/
class ClippingClass {
  public:
    void threshold(uint16_t value) {                                //Порог "проклипывания"
      if (value > 2048)
        value = 2048;
      _threshold = value;
    }
    void smoothness(uint16_t value) {                               //Плавность (мягкость)
      if (value > 4095)
        value = 4095;
      _smoothness = value;
    }
    uint16_t get(uint16_t value) {                                  //Получение значения. Алгоритм обрезания сигнала по порогу со смягчением (нежёсткая обрезка волны по линии лимита), установленным функцией smoothness()
      if (value >= 2048) {                                          //Если значение больше половины
        if (value > 4095 - _threshold) {                            //И значение больше "отступа" (разность от макс. значения и порога)
          if (_smoothness)                                          //Если сглаживание > 0
            value = (4095 - _threshold) + ((value - (4095 - _threshold)) / map(_smoothness, 1, 4095, (4095 - _threshold), 1));          //Значение = порог (отступ) + разность значения, превыщающего порог, и самого порога / (преведение значения сглаживания к значению из промежутка [разность макс. значения и порога (интервал значений сигнала для приведения), 1 (отсутствие эффекта)])
          else
            value = 4095 - _threshold;                              //Иначе (сглаживание = 0) обрезать по порогу
        }
      } else {
        if (value < _threshold) {                                   //Если значение меньше порога, то выполнить тоже самое, что и в коде выше, но отобразив точку значения на графике сигнала по горизонтали относительно значения 2048
          if (_smoothness)
            value = _threshold - ((_threshold - value) / map(_smoothness, 1, 4095, (2048 - _threshold), 1));
          else
            value = _threshold;
        }
      }
      return value;
    }
  private:
    uint16_t _threshold = 0;
    uint16_t _smoothness = 0;
};
static ClippingClass Clipping;                                      //Статический класс, т. к. нет смысла создавать обычный - не нужно разделять на объекты, не выделяется память под динамический массив
