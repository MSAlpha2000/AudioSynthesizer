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
