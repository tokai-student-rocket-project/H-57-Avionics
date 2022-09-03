#ifndef LPS22HB_H
#define LPS22HB_H

#define LPS22HB_ADDRESS_AD0_LOW 0x5C
#define LPS22HB_ADDRESS_AD0_HIGH 0x5D
#define LPS22HB_DEFAULT_ADDRESS LPS22HB_ADDRESS_AD0_LOW

#define LPS22HB_CONTROL_REGISTER 0x10
#define LPS22HB_CONTROL_75HZ 0x50

#define LPS22HB_PRESSURE_OUT 0x28
#define LPS22HB_TEMPERATURE_OUT 0x2B

class LPS22HB
{
public:
  /// @brief コンストラクタ
  /// @param deviceAddress AD0がGNDに接続: LPS22HB_ADDRESS_AD0_LOW, AD0がVDDIOに接続: LPS22HB_ADDRESS_AD0_HIGH
  LPS22HB(int deviceAddress);

  /// @brief 75Hzで駆動するよ
  void initialize();

  /// @brief 気圧を取得する
  /// @return 気圧[hPa]
  double getPressure();

  /// @brief 基準気圧と基準気温を設定する
  /// @param basePressure 基準気圧。ランチャー付近の気圧でいいと思う。
  /// @param baseTemperature 基準気温。ランチャー付近の気温でいいと思う。
  void setConfig(double basePressure, double beseTemperature);
  /// @brief 高度を取得する
  /// @return 高度[m]
  double getAltitude();

private:
  int _deviceAddress;
  // 一応初期値として標準大気を入れておく
  double _basePressure = 1013;
  double _baseTemperature = 15;
};

#endif