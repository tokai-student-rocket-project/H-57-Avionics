#ifndef DESCENT_DETECTOR_H_
#define DESCENT_DETECTOR_H_

/// @brief 機体が降下しているかを検出するクラス。updateAltitude()で高度を更新して降下していれば_isDescendingがtrueになる。
class DescentDetector
{
public:
  /// @brief コンストラクタ
  /// @param sensitivity 大きいほど強い平滑化(0~1)
  DescentDetector(double sensitivity);

  /// @brief 高度を更新します。値に応じて_isDescendinも更新されます
  /// @param altitude 高度
  void updateAltitude(double altitude);

  /// @brief 降下中か
  bool _isDescending;
  
private:
  /// @brief メートルで指定。この値以上に減少しないと降下にならない
  const double DESCENT_DIFFERENT = 0.0;
  /// @brief 回数で指定。この値以上に連続で減少しないと降下にならない
  const int MINIMUM_DESCENT_COUNT = 10;

  double _sensitivity;
  double _average;
  double _averageOrigin;
  int _descentCount;
};

#endif