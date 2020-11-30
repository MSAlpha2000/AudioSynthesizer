![Синтезатор на Ардуино Synthesizer on Arduino](https://github.com/MatInnovation/AudioSynthesizer/blob/main/AudioSynthesizer.png)
# Аудио синтезатор
Библиотека аудио синтезатор для Arduino DUE
## Функции:
* Быстрый аналоговый ввод / вывод
* Стерео / моно аудио выход
* Выбор моно канала
* Внутренний / внешний аудио процессор
* Память на ноты с множественными несущими волнами
* Осциллятор (генератор волн)
* Огибающая (амплитудная / частотная)
* Вибратто (тремоло (амплитудное) / бенд (частотное))
* Легато
* Стакатто
* Питч шифтер (изменение тона ноты)
* Звуковые эффекты:
    * Отсекание (клиппинг) (жёсткий (искажение) / мягкий (перегрузка))
    * Задержка
    * Хор
    * Флэнжер
    * Фазовращатель (фейзер)
## Преимущества:
* Широкие возможности (набор профессиональных функций)
* Цифровой синтез **абсолютно любого звука**
* Большое количество несущих частот на одну ноту
* Наличие в библиотеке примера - **работающего синтезатора**
* Лёгкость использования
* Интуитивно понятные функции
* Хорошо прокоментированный код
* Простота установки
## Пучему стоит выбрать именно эту библиотеку для создания своего ситезатора
```
Вам не нужно ничего придумывать самостоятельно, мы уже всё организовали за Вас!
```
Всё, что требуется от пользователя - подключить библиотеку и наслаждаться синтезатором, близким по своему функционалу к профессиональному.
## Четыре причины использовать эту библиотеку для разработки собственного програмного кода
* Програмный код написан качественно
* Используются понятные имена функций и переменных
* Удобная и понятная структура кода
* Библиотека даёт возможность как использовать отдельные её классы, так и вмешеваться в любую стадию создания и обработки звука - **для этого уже созданы все необходимые функции**
## Подключение библиотеки в Arduino IDE
Скетч -> Подключить библиотеку -> Добавить .ZIP библиотеку...
## Особенности библиотеки
Библиотека написана только для работы на Arduino DUE
## Пример использования
```С++
#define OPTIMIZATION_DELAY                                                              //Оптимизируем для нужных параметров
#include <AudioSynthesizer.hpp>                                                         //Подключим заголовочный файл библиотеки
#include <iarduino_KB.h>                                                                //Подключим библиотеку для работы с матричной клавиатурой (сторонняя библиотека от iarduino)
iarduino_KB KB (2, 3, 4, 5, 6, 7, 8, 9);                                                //Клавиатура
bool mode = true;                                                                       //Режим (играем / не играем (настраиваем параметры)
bool select = false;                                                                    //Выбор (какие параметры настраиваем)
uint8_t group = 0;                                                                      //Группа нот (A / B / C / D)
uint8_t noteChanel = 0;                                                                 //Номер ноты
uint8_t waveChanel = 0;                                                                 //Номер волны в ноте
//Типы волн
uint8_t vibrattoWaveType = 0;
uint8_t chorusWaveType = 0;
uint8_t flangerWaveType = 0;
uint8_t phaserWaveType = 0;
void setup() {
  //Настраиваем кнопки
  pinMode(51, INPUT_PULLUP);
  pinMode(46, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(50, INPUT);
  pinMode(15, INPUT);
  pinMode(16, INPUT);
  pinMode(17, INPUT);
  pinMode(18, INPUT);
  pinMode(19, INPUT);
  pinMode(22, OUTPUT);
  KB.begin(KB1);                                                                        //Матричная клавиатура 4х4
  AudioSynthesizer.enable(40000, 40, 8, USE_ANALOG_OUTPUT, USE_ANALOG_INPUT, STEREO);   //Синтезатор (частота дискретизации 40 кГц, 40 нот, 8 несущих волн в ноте, используем ЦАП и АЦП, стерео)
}
void loop() {
  if (KB.check(KEY_DOWN)) {                                                             //Если нажата клавиша на клавиатуре
    uint8_t key = KB.getNum;                                                            //Получим её номер
    if (key >= 14) {                                                                    //Если это * или #
      if (key == 14)                                                                    //*
        mode = !mode;                                                                   //Меняем режим
      else                                                                              //#
        select = !select;                                                               //Меняем выбор
    } else {
      if (key >= 10) {                                                                  //Клавиши с буквами
        if (mode) {                                                                     //Если играем
          group = key - 10;                                                             //Группа нот
        } else {                                                                        //Если не играем
          if (select) {                                                                 //Если выбираем
            //Тип волны для разных эффектов
            if (!digitalRead(42)) {
              chorusWaveType = key - 10;
            } else {
              if (!digitalRead(44)) {
                flangerWaveType = key - 10;
              } else {
                if (!digitalRead(46)) {
                  phaserWaveType = key - 10;
                } else {
                  vibrattoWaveType = key - 10;
                }
              }
            }
          } else {                                                                      //Если не выбираем
            AudioSynthesizer.waveType(noteChanel, waveChanel, key - 10);                //Тип волны для волны ноты
          }
        }
      } else {                                                                          //Если нажата цифра
        if (mode) {                                                                     //Если играем
          noteChanel = group * 10 + key;                                                //Номер ноты
          AudioSynthesizer.note(noteChanel);                                            //Играем ноту
        } else {                                                                        //Если не играем
          waveChanel = key;                                                             //Номер волны = номеру клавиши
        }
      }
    }
  }
  if (KB.check(KEY_UP))                                                                 //Если клавиша отпущена
    AudioSynthesizer.note();                                                            //Отпускаем ноту
  if (mode) {                                                                           //Если играем
    //Настраиваем волны эффектов
    if (!digitalRead(42)) {
      AudioSynthesizer.chorusWave(chorusWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(14));
    }
    if (!digitalRead(44)) {
      AudioSynthesizer.flangerWave(flangerWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(14));
    }
    if (!digitalRead(46)) {
      AudioSynthesizer.phaserWave(phaserWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(14));
    }
    //Настаиваем эффекты
    AudioSynthesizer.delay(map(AudioSynthesizer.input(A0), 0, 4095, 0, 1000000), AudioSynthesizer.input(A1), AudioSynthesizer.input(A2));
    AudioSynthesizer.chorus(AudioSynthesizer.input(A3), AudioSynthesizer.input(A4));
    AudioSynthesizer.flanger(AudioSynthesizer.input(A5), AudioSynthesizer.input(A6));
    AudioSynthesizer.phaser(AudioSynthesizer.input(D52), AudioSynthesizer.input(D21));
    AudioSynthesizer.clipping(map(AudioSynthesizer.input(A8), 0, 4095, 0, 2048), AudioSynthesizer.input(A9));
    AudioSynthesizer.usePitchShifter(digitalRead(16));
    AudioSynthesizer.pitchShifter(map(AudioSynthesizer.input(A7), 0, 4095, -20000, 20000));
    AudioSynthesizer.useTremolo(digitalRead(15));
    AudioSynthesizer.useBend(digitalRead(17));
    if (digitalRead(50))                                                                //Выбираем легато / стакатто
      AudioSynthesizer.legato(!digitalRead(51));                                        //Легато (кнопка нажата)
    else
      AudioSynthesizer.stakatto(!digitalRead(51));                                      //Стакатто (кнопка нажата)
  } else {                                                                              //Если не играем
    if (select) {                                                                       //Если выбираем параметры
      //Настраиваем огибающую
      AudioSynthesizer.useEnvelope(AM, digitalRead(15));
      AudioSynthesizer.useEnvelope(FM, digitalRead(17));
      AudioSynthesizer.envelope(map(AudioSynthesizer.input(A0), 0, 4095, -2048, 2048), map(AudioSynthesizer.input(A1), 0, 4095, -2048, 2048), map(AudioSynthesizer.input(A2), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A3), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A4), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A5), 0, 4095, -2048, 2048), map(AudioSynthesizer.input(A6), 0, 4095, 0, 1000), AM);
      AudioSynthesizer.envelope(map(AudioSynthesizer.input(A8), 0, 4095, -20000, 20000), map(AudioSynthesizer.input(A9), 0, 4095, -20000, 20000), map(AudioSynthesizer.input(A10), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 1000), map(AudioSynthesizer.input(D20), 0, 4095, 0, 1000), map(AudioSynthesizer.input(D52), 0, 4095, -20000, 20000), map(AudioSynthesizer.input(D21), 0, 4095, 0, 1000), FM);
      AudioSynthesizer.phaserFreq(AudioSynthesizer.input(A7));                          //Частота фазовращателя
    } else {                                                                            //Если не выбираем (выбираем другие параметры)
      //Выбираем вибратто (тремоло / бенд)
      if (digitalRead(14)) {
        AudioSynthesizer.tremolo(vibrattoWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(50));
      } else {
        AudioSynthesizer.bend(vibrattoWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(50));
      }
      //Настраиваем амплитуду / частоту / фазу
      if (digitalRead(15)) {
        if (digitalRead(17)) {
          AudioSynthesizer.waveAmpl(noteChanel, 0, map(AudioSynthesizer.input(A0), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 1, map(AudioSynthesizer.input(A1), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 2, map(AudioSynthesizer.input(A2), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 3, map(AudioSynthesizer.input(A3), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 4, map(AudioSynthesizer.input(A4), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 5, map(AudioSynthesizer.input(A5), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 6, map(AudioSynthesizer.input(A6), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 7, map(AudioSynthesizer.input(A7), 0, 4095, 0, 2048));
        } else {
          AudioSynthesizer.wavePhase(noteChanel, 0, AudioSynthesizer.input(A0), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 1, AudioSynthesizer.input(A1), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 2, AudioSynthesizer.input(A2), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 3, AudioSynthesizer.input(A3), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 4, AudioSynthesizer.input(A4), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 5, AudioSynthesizer.input(A5), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 6, AudioSynthesizer.input(A6), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 7, AudioSynthesizer.input(A7), digitalRead(51));
        }
      } else {
        AudioSynthesizer.waveFreq(noteChanel, 0, map(AudioSynthesizer.input(A0), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 1, map(AudioSynthesizer.input(A1), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 2, map(AudioSynthesizer.input(A2), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 3, map(AudioSynthesizer.input(A3), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 4, map(AudioSynthesizer.input(A4), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 5, map(AudioSynthesizer.input(A5), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 6, map(AudioSynthesizer.input(A6), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 7, map(AudioSynthesizer.input(A7), 0, 4095, 0, 20000));
      }
      AudioSynthesizer.volume(AudioSynthesizer.input(A8));                              //Громкость
    }
  }
  AudioSynthesizer.audioOutputMode(digitalRead(18), digitalRead(19));                   //Стерео / моно, канал
  AudioSynthesizer.process();                                                           //Обрабатываем сигнал
  AudioSynthesizer.output(INTERNAL_SOURCE);                                             //Выводим с внутреннего источника (внутреннего процессора)
  digitalWrite(22, AudioSynthesizer.getProcess());                                      //Индикатора обработки сигнала
}
```
## Как помочь проекту?
Мы искренне надеемся, что написанная нами библиотека окажется полезной для Вас.
Автор проекта не приследует никакие коммерческие цели - **продукт создан исключительно во благо людей**.
Програмный код проекта распространяется открыто и совершенно бесплатно.
Проект призван помочь музыкантам-любителям и начинающим музыкантам, заменив дорогостоящий проффесиональный синтезатор дешёвым, но оправдывающим себя, аналогом на платформе Arduino.
Все предложения по улучшению / доработке, комментарии и т. д. пишите нам на электронную почту rozmari1123@gmail.com.
Мы постараемся оперативно реагировать на все Ваши предложения.
`Мы будем Вам очень признательны если Вы поддержите проект ЛЮБЫМ способом. Например, Вы можете поделиться им с друзьями и знакомыми, распространить в соц. сетях или сделать обзор на продукт в своём блоге или на канале.`
**Вместе мы можем создавать будущее и менять мир к лучшему!**

# Audio Synthesizer
Audio Synthesizer Lib for Arduino DUE
## Functions:
* FAST ANALOG INPUT / OUTPUT
* STEREO / MONO AUDIO OUTPUT
* MONO CHANNEL SELECTION
* INTERNAL / EXTERNAL AUDIO PROCESSOR
* MEMORY FOR NOTES WITH MULTIPLE CARRIER WAVES
* OSCILLATOR
* ENVELOPE (AMPLITUDE / FREQUENCY)
* VIBRATTO (TREMOLO (AMPLITUDE) / BEND (FREQUENCY))
* LEGATO
* STAKATTO
* PITCH SHIFTER
* SOUND EFFECTS:
* CLIPPING (HARD (DISTORTION) / SOFT (OVERLOAD))
* DELAY
* CHORUS
* FLANGER
* PHASER
## Advantages:
* Extensive features (a set of professional functions)
* Digital synthesis * * absolutely any sound**
* A large number of carrier frequencies on a single note
* The library has an example - * * working synthesizer**
* Ease of use
* Intuitive features
* Well-commented code
* Easy installation
## Pochemu should choose this library to create your sitestore
```
You don't need to come up with anything yourself, we have already organized everything for You!
```
All that is required from the user is to connect the library and enjoy the synthesizer, which is close in its functionality to the professional one.
## Four reasons to use this library to develop your own code
* The program code is written efficiently
* Clear names of functions and variables are used
* Convenient and clear code structure
* The library allows you to use its individual classes, as well as to interfere with any stage of sound creation and processing - **all the necessary functions have already been created for this purpose**
## Connecting the library in the Arduino IDE
Sketch -> Connect library -> Add .ZIP library...
## library Features
The library is written only to work on Arduino DUE
## Example of use
```С++
#define OPTIMIZATION_DELAY                                                              //Оптимизируем для нужных параметров
#include <AudioSynthesizer.hpp>                                                         //Подключим заголовочный файл библиотеки
#include <iarduino_KB.h>                                                                //Подключим библиотеку для работы с матричной клавиатурой (сторонняя библиотека от iarduino)
iarduino_KB KB (2, 3, 4, 5, 6, 7, 8, 9);                                                //Клавиатура
bool mode = true;                                                                       //Режим (играем / не играем (настраиваем параметры)
bool select = false;                                                                    //Выбор (какие параметры настраиваем)
uint8_t group = 0;                                                                      //Группа нот (A / B / C / D)
uint8_t noteChanel = 0;                                                                 //Номер ноты
uint8_t waveChanel = 0;                                                                 //Номер волны в ноте
//Типы волн
uint8_t vibrattoWaveType = 0;
uint8_t chorusWaveType = 0;
uint8_t flangerWaveType = 0;
uint8_t phaserWaveType = 0;
void setup() {
  //Настраиваем кнопки
  pinMode(51, INPUT_PULLUP);
  pinMode(46, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(50, INPUT);
  pinMode(15, INPUT);
  pinMode(16, INPUT);
  pinMode(17, INPUT);
  pinMode(18, INPUT);
  pinMode(19, INPUT);
  pinMode(22, OUTPUT);
  KB.begin(KB1);                                                                        //Матричная клавиатура 4х4
  AudioSynthesizer.enable(40000, 40, 8, USE_ANALOG_OUTPUT, USE_ANALOG_INPUT, STEREO);   //Синтезатор (частота дискретизации 40 кГц, 40 нот, 8 несущих волн в ноте, используем ЦАП и АЦП, стерео)
}
void loop() {
  if (KB.check(KEY_DOWN)) {                                                             //Если нажата клавиша на клавиатуре
    uint8_t key = KB.getNum;                                                            //Получим её номер
    if (key >= 14) {                                                                    //Если это * или #
      if (key == 14)                                                                    //*
        mode = !mode;                                                                   //Меняем режим
      else                                                                              //#
        select = !select;                                                               //Меняем выбор
    } else {
      if (key >= 10) {                                                                  //Клавиши с буквами
        if (mode) {                                                                     //Если играем
          group = key - 10;                                                             //Группа нот
        } else {                                                                        //Если не играем
          if (select) {                                                                 //Если выбираем
            //Тип волны для разных эффектов
            if (!digitalRead(42)) {
              chorusWaveType = key - 10;
            } else {
              if (!digitalRead(44)) {
                flangerWaveType = key - 10;
              } else {
                if (!digitalRead(46)) {
                  phaserWaveType = key - 10;
                } else {
                  vibrattoWaveType = key - 10;
                }
              }
            }
          } else {                                                                      //Если не выбираем
            AudioSynthesizer.waveType(noteChanel, waveChanel, key - 10);                //Тип волны для волны ноты
          }
        }
      } else {                                                                          //Если нажата цифра
        if (mode) {                                                                     //Если играем
          noteChanel = group * 10 + key;                                                //Номер ноты
          AudioSynthesizer.note(noteChanel);                                            //Играем ноту
        } else {                                                                        //Если не играем
          waveChanel = key;                                                             //Номер волны = номеру клавиши
        }
      }
    }
  }
  if (KB.check(KEY_UP))                                                                 //Если клавиша отпущена
    AudioSynthesizer.note();                                                            //Отпускаем ноту
  if (mode) {                                                                           //Если играем
    //Настраиваем волны эффектов
    if (!digitalRead(42)) {
      AudioSynthesizer.chorusWave(chorusWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(14));
    }
    if (!digitalRead(44)) {
      AudioSynthesizer.flangerWave(flangerWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(14));
    }
    if (!digitalRead(46)) {
      AudioSynthesizer.phaserWave(phaserWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(14));
    }
    //Настаиваем эффекты
    AudioSynthesizer.delay(map(AudioSynthesizer.input(A0), 0, 4095, 0, 1000000), AudioSynthesizer.input(A1), AudioSynthesizer.input(A2));
    AudioSynthesizer.chorus(AudioSynthesizer.input(A3), AudioSynthesizer.input(A4));
    AudioSynthesizer.flanger(AudioSynthesizer.input(A5), AudioSynthesizer.input(A6));
    AudioSynthesizer.phaser(AudioSynthesizer.input(D52), AudioSynthesizer.input(D21));
    AudioSynthesizer.clipping(map(AudioSynthesizer.input(A8), 0, 4095, 0, 2048), AudioSynthesizer.input(A9));
    AudioSynthesizer.usePitchShifter(digitalRead(16));
    AudioSynthesizer.pitchShifter(map(AudioSynthesizer.input(A7), 0, 4095, -20000, 20000));
    AudioSynthesizer.useTremolo(digitalRead(15));
    AudioSynthesizer.useBend(digitalRead(17));
    if (digitalRead(50))                                                                //Выбираем легато / стакатто
      AudioSynthesizer.legato(!digitalRead(51));                                        //Легато (кнопка нажата)
    else
      AudioSynthesizer.stakatto(!digitalRead(51));                                      //Стакатто (кнопка нажата)
  } else {                                                                              //Если не играем
    if (select) {                                                                       //Если выбираем параметры
      //Настраиваем огибающую
      AudioSynthesizer.useEnvelope(AM, digitalRead(15));
      AudioSynthesizer.useEnvelope(FM, digitalRead(17));
      AudioSynthesizer.envelope(map(AudioSynthesizer.input(A0), 0, 4095, -2048, 2048), map(AudioSynthesizer.input(A1), 0, 4095, -2048, 2048), map(AudioSynthesizer.input(A2), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A3), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A4), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A5), 0, 4095, -2048, 2048), map(AudioSynthesizer.input(A6), 0, 4095, 0, 1000), AM);
      AudioSynthesizer.envelope(map(AudioSynthesizer.input(A8), 0, 4095, -20000, 20000), map(AudioSynthesizer.input(A9), 0, 4095, -20000, 20000), map(AudioSynthesizer.input(A10), 0, 4095, 0, 1000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 1000), map(AudioSynthesizer.input(D20), 0, 4095, 0, 1000), map(AudioSynthesizer.input(D52), 0, 4095, -20000, 20000), map(AudioSynthesizer.input(D21), 0, 4095, 0, 1000), FM);
      AudioSynthesizer.phaserFreq(AudioSynthesizer.input(A7));                          //Частота фазовращателя
    } else {                                                                            //Если не выбираем (выбираем другие параметры)
      //Выбираем вибратто (тремоло / бенд)
      if (digitalRead(14)) {
        AudioSynthesizer.tremolo(vibrattoWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(50));
      } else {
        AudioSynthesizer.bend(vibrattoWaveType, map(AudioSynthesizer.input(A10), 0, 4095, 0, 20000), map(AudioSynthesizer.input(A11), 0, 4095, 0, 2048), AudioSynthesizer.input(D20), digitalRead(50));
      }
      //Настраиваем амплитуду / частоту / фазу
      if (digitalRead(15)) {
        if (digitalRead(17)) {
          AudioSynthesizer.waveAmpl(noteChanel, 0, map(AudioSynthesizer.input(A0), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 1, map(AudioSynthesizer.input(A1), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 2, map(AudioSynthesizer.input(A2), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 3, map(AudioSynthesizer.input(A3), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 4, map(AudioSynthesizer.input(A4), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 5, map(AudioSynthesizer.input(A5), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 6, map(AudioSynthesizer.input(A6), 0, 4095, 0, 2048));
          AudioSynthesizer.waveAmpl(noteChanel, 7, map(AudioSynthesizer.input(A7), 0, 4095, 0, 2048));
        } else {
          AudioSynthesizer.wavePhase(noteChanel, 0, AudioSynthesizer.input(A0), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 1, AudioSynthesizer.input(A1), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 2, AudioSynthesizer.input(A2), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 3, AudioSynthesizer.input(A3), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 4, AudioSynthesizer.input(A4), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 5, AudioSynthesizer.input(A5), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 6, AudioSynthesizer.input(A6), digitalRead(51));
          AudioSynthesizer.wavePhase(noteChanel, 7, AudioSynthesizer.input(A7), digitalRead(51));
        }
      } else {
        AudioSynthesizer.waveFreq(noteChanel, 0, map(AudioSynthesizer.input(A0), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 1, map(AudioSynthesizer.input(A1), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 2, map(AudioSynthesizer.input(A2), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 3, map(AudioSynthesizer.input(A3), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 4, map(AudioSynthesizer.input(A4), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 5, map(AudioSynthesizer.input(A5), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 6, map(AudioSynthesizer.input(A6), 0, 4095, 0, 20000));
        AudioSynthesizer.waveFreq(noteChanel, 7, map(AudioSynthesizer.input(A7), 0, 4095, 0, 20000));
      }
      AudioSynthesizer.volume(AudioSynthesizer.input(A8));                              //Громкость
    }
  }
  AudioSynthesizer.audioOutputMode(digitalRead(18), digitalRead(19));                   //Стерео / моно, канал
  AudioSynthesizer.process();                                                           //Обрабатываем сигнал
  AudioSynthesizer.output(INTERNAL_SOURCE);                                             //Выводим с внутреннего источника (внутреннего процессора)
  digitalWrite(22, AudioSynthesizer.getProcess());                                      //Индикатора обработки сигнала
}
```
## How can I help the project?
We sincerely hope that the library we have written will be useful for You.
The author of the project does not pursue any commercial goals - * * the product was created exclusively for the benefit of people**.
The program code of the project is distributed openly and completely free of charge.
The project is designed to help Amateur musicians and novice musicians by replacing an expensive professional synthesizer with a cheap, but justified, analog on the Arduino platform.
All suggestions for improvement / improvement, comments, etc. write to us by email rozmari1123@gmail.com.
We will try to respond promptly to all your suggestions.
`We will be very grateful if you support the project in ANY way. For example, you can share it with friends and acquaintances, distribute it in social networks. or make a review of the product on your blog or channel.`
**Together we can create the future and change the world for the better!**
