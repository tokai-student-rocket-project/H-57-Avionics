#include <Arduino.h>
#include "DescentDetector.h"

/// @brief コンストラクタ
/// @param sensitivity 大きいほど強い平滑化(0~1)
DescentDetector::DescentDetector(double sensitivity) {
  _sensitivity = sensitivity;
}
/// @brief 高度を更新します。値に応じて_isDescendinも更新されます
/// @param altitude 高度
void DescentDetector::updateAltitude(double altitude)
{
  // 指数平滑化移動平均
  _average = _averageOrigin + _sensitivity * (altitude - _averageOrigin);

  // 平滑化された高度が減少すれば_descentCountをインクリメントする
  // 減少しなければリセット
  if ((_averageOrigin - _average) > DESCENT_DIFFERENT) {
    ++_descentCount;
  }
  else {
    _descentCount = 0;
  }

  // _descentCountが指定回数(MINIMUM_DESCENT_COUNT)より多くなれば(=指定回数以上に連続で高度が減少)降下していると判断する。
  _isDescending = _descentCount >= MINIMUM_DESCENT_COUNT;

  _averageOrigin = _average;

  // デバッグ用
  // 後で消す
  // Serial.print(altitude);
  // Serial.print("\t");
  // Serial.print(_average);
  // Serial.print("\t");
  // Serial.print(_descentCount);
  // Serial.print("\t");
  // Serial.println(10);
}
