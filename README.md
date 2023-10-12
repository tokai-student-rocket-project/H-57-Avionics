# H-57 搭載計器

![2022年度 - 冬季](https://img.shields.io/badge/2022年度-冬季-blue)
![射点 - 大樹](https://img.shields.io/badge/射点-北海道/大樹町-orange)
![形式 - 陸打ち](https://img.shields.io/badge/形式-陸打ち-green)

TSRP-H-57に搭載する計器類のプログラム及び設計書を保存するレポジトリです。

![Hero](./Documents/Images/Hero.png)

## はじめに

学生ロケットの電装系を盛り上げたい思いから、全てのソースコードを含む設計を公開しています。ガバガバ開発ですので、分かりにくい点（資料がまとまっていないなど）が多々あります。ご了承ください。

ご不明な点は以下のSNSからお気軽にお問合せください。

- X (旧Twitter): [@Tokai_SRP](https://twitter.com/Tokai_SRP)

### 計器の概要


H-57搭載計器は新型共通計器基板、テレメータ基板、バルブ制御基板および、電源基板から構成されます。

### レポジトリ構成

#### ./FlightComponents/

- [新型共通計器](./FlightComponents/CommonInstrument/README.md) ![通信系](https://img.shields.io/badge/通信系-blue) ![センサ系](https://img.shields.io/badge/センサ系-green) ![分離制御系](https://img.shields.io/badge/分離制御系-yellow)
- [テレメータ](./FlightComponents/Telemeter/README.md) ![通信系](https://img.shields.io/badge/通信系-blue) ![センサ系](https://img.shields.io/badge/センサ系-green)
- [バルブ制御](./FlightComponents/Telemeter/README.md) ![バルブ制御系](https://img.shields.io/badge/バルブ制御系-orange)
- 電源 ![電源系](https://img.shields.io/badge/電源系-red)

#### ./FlightResult/

- [解析資料](./FlightResult/CommonInstrument/H-57_Avionics_Analysis.md)