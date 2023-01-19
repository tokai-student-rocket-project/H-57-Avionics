#include <Arduino.h>
#include "DescentDetector.h"


/// @brief 高度を更新します。値に応じて_isDescendinも更新されます
/// @param altitude 高度
void DescentDetector::updateAltitude(float altitude)
{
  // 指数平滑化移動平均
  _average = _averageOrigin + SENSITIVITY * (altitude - _averageOrigin);

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
}
