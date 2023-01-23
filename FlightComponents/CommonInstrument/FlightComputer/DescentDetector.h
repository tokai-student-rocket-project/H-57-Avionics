#ifndef DESCENT_DETECTOR_H_
#define DESCENT_DETECTOR_H_

/// @brief 機体が降下しているかを検出するクラス。updateAltitude()で高度を更新して降下していれば_isDescendingがtrueになる。
class DescentDetector
{
public:
  /// @brief 高度を更新します。値に応じて_isDescendinも更新されます
  /// @param altitude 高度
  void updateAltitude(float altitude);

  /// @brief 降下中か
  bool _isDescending;

  uint16_t _descentCount;

private:
  /// @brief メートルで指定。この値以上に減少しないと降下にならない
  const float DESCENT_DIFFERENT = 0.0;
  /// @brief 回数で指定。この値以上に連続で減少しないと降下にならない
  const uint16_t MINIMUM_DESCENT_COUNT = 10;
  // 高度平滑化の強度。手元の試験では0.35がちょうどよかった。
  const float SENSITIVITY = 0.25;

  float _average;
  float _averageOrigin;
};

#endif