#include <Arduino.h>
#include "FrequencyTimer.h"


/// @brief コンストラクタ
/// @param frameRate 周期 [Hz]
FrequencyTimer::FrequencyTimer(int frameRate) {
  _delayTime_us = (unsigned int)(1000000 / frameRate);
}


/// @brief loopの最後で呼び出す
void FrequencyTimer::delay() {
  // 前回delayが呼び出されてからの経過時間を求める
  unsigned int processTime_us = micros() - _originTime_us;
  unsigned int trueDelayTime_us = _delayTime_us - processTime_us;

  // trueDelayTime_usがオーバーフローしている（=loop中の処理時間が_delayTime_usよりも長い）場合は遅延しない。
  // この場合は指定したフレームレートよりも落ちるが致し方なし。
  if (trueDelayTime_us <= _delayTime_us) delayMicroseconds(trueDelayTime_us);

  _originTime_us = micros();
}