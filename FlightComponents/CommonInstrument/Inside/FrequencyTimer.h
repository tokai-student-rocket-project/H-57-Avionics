#ifndef FREQUENCY_TIMER_H_
#define FREQUENCY_TIMER_H_

/// @brief loopを決まった周期で回すクラス。loopの最後でdelayを呼び出す。
class FrequencyTimer {
private:
  unsigned int _delayTime_us;
  unsigned int _originTime_us;

public:
  /// @brief コンストラクタ
  /// @param frameRate 周期 [Hz]
  FrequencyTimer(int frameRate);

  /// @brief loopの最後で呼び出す
  void delay();
};


#endif