#pragma once
/*
   Класс для работы с ЦАП
   Обеспечивает быстрый вывод на аналоговый контакт
   Быстрее, чем функция analogWrite(), т. к. работает напрямую с регистрами
*/
class DACInterfaceClass {
  public:
    void init() {                                           //Инициализация. Работает напрямую с регистрами микроконтроллера, поэтому здесь можно не вчитываться :) Для более подробной информации читать даташит на процессор
      PMC -> PMC_PCER1 = 1 << (DACC_INTERFACE_ID - 32);
      DACC -> DACC_CR = DACC_CR_SWRST;
      DACC -> DACC_MR = DACC_MR_WORD | DACC_MR_TAG | DACC_MR_MAXS | 256;
      DACC -> DACC_IER = DACC_IER_TXRDY;
      DACC -> DACC_CHER = DACC_CHER_CH0 | DACC_CHER_CH1;
    }
    void write(uint16_t _data0ch = 2048, uint16_t _data1ch = 2048) {              //Устанавливает на контактах DAC0 и DAC1 значения, переданные в качестве параметров
      if (_data0ch > 4095 or _data1ch > 4095)
        return;
      while (!(DACC -> DACC_ISR & DACC_ISR_TXRDY));                               //Ждём готовности интерфейса
      DACC -> DACC_CDR = (_data0ch  + (_data1ch << 16)) | 0x10000000;             //Запись значений
    }
};
static DACInterfaceClass DACInterface;                      //Класс должен быть статическим, т. к. у нас только один аппаратный интерфейс
