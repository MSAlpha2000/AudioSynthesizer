#pragma once
/*
   Буффер
   Вспомогательный класс
*/
class Buffer {
  public:
    Buffer (uint32_t length) {
      _length = length;
      _buffer = new uint16_t[_length];
      memset(_buffer, 2048, sizeof(uint16_t) * _length);
    }
    ~Buffer () {
      delete[] _buffer;
    }
    uint16_t get(uint32_t index) {                                //Подучение элемента массива
      if (index >= _length)
        return 2048;
      _index = index;
      return _buffer[_index];
    }
    void put(uint16_t value) {                                    //Ставим значение в начало, остальные элементы сдвигаем на один
      for (uint32_t i = _index; i > 0; i--)
        _buffer[i] = _buffer[i - 1];
      _buffer[0] = value;
    }
  private:
    uint32_t _length = 0;
    uint32_t _index = 0;
    uint16_t* _buffer;
};
