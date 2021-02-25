#pragma once
/*
   Класс для работы с АЦП
   Обеспечивает быстрый ввод с аналогового контакта
   Быстрее, чем функция analogRead(), т. к. работает напрямую с регистрами
*/
class ADCInterfaceClass {
  public:
    void init() {                                           //Инициализация. Работает напрямую с регистрами микроконтроллера, поэтому здесь можно не вчитываться :) Для более подробной информации читать даташит на процессор
      ADC -> ADC_CR = ADC_CR_SWRST;
      ADC -> ADC_MR = ADC_MR_FREERUN;
      ADC -> ADC_EMR = ADC_EMR_TAG;
      ADC -> ADC_IER = ADC_IER_DRDY;
      ADC -> ADC_CHER = ADC_CHER_CH0 | ADC_CHER_CH1 | ADC_CHER_CH2 | ADC_CHER_CH3 | ADC_CHER_CH4 | ADC_CHER_CH5 | ADC_CHER_CH6 | ADC_CHER_CH7 | ADC_CHER_CH8 | ADC_CHER_CH9 | ADC_CHER_CH10 | ADC_CHER_CH11 | ADC_CHER_CH12 | ADC_CHER_CH13 | ADC_CHER_CH14 | ADC_CHER_CH15;
      ADC -> ADC_CR = ADC_CR_START;
    }
    uint8_t read() {                                        //(void)  Возвращает номер канала, с которого было прочитано значение
      while (!(ADC -> ADC_ISR & ADC_ISR_DRDY));             //Ждём регистр прерываний
      uint16_t val = ADC -> ADC_LCDR;
      uint8_t channel = val >> 12;                          //Получаем номер канала
      _data = val - (channel << 12);                        //Получаем значение
      return channel;
    }
    uint16_t get() {                                        //Возвращает значение, прочитанное с канала, номер которго вернула функция read(void)
      return _data;
    }
    uint16_t read(uint8_t channel) {                        //Возвращает значение, прочитанное с канала, переданного в качестве параметра
      if (channel > 15)
        return 0;
      while (read() != channel);                            //Ждём, пока функция read() без параметров вернёт номер нужного канала
      return get();                                         //Возвращаем прочитанное значение
    }
  private:
    uint16_t _data = 0;
};
static ADCInterfaceClass ADCInterface;                      //Класс должен быть статическим, т. к. у нас только один аппаратный интерфейс
