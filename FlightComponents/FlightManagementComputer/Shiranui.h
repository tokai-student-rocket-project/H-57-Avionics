#ifndef SHIRANUI_H_
#define SHIRANUI_H_

/// @brief 不知火の抽象化クラス
class Shiranui
{
public:
  /// @brief コンストラクタ
  /// @param pin Arduinoのデジタルピン
  Shiranui(int pin);

  /// @brief 不知火を初期化する
  void initialize();

  /// @brief 不知火に分離指令を出す
  void separate();

  /// @brief 不知火の分離指令をリセットします
  void reset();
  
private:
  int _pin;
};

#endif