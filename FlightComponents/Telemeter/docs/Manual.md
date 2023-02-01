# 遠隔測定モジュール作業手順書

1. UARTケーブルの接続
   - 用意するもの
     - UART用4線ケーブル × 1
       - 色の配置：1- <span style="color: yellow; ">黄</span>,<span style="color: green; ">緑</span>,<span style="color: black; ">黒</span>,<span style="color: red; ">赤</span> -4


> 基本的に外すことはないのでもしも外れていたらこの手順を踏む。
> ![UART](Manual_Telematar_UART.JPG "UARTケーブルの接続")
>
***

2. 状態確認LEDの接続
    - 用意するもの
      - LED付きケーブル × 2
        - UART接続確認用LED(<span style="color: green; ">緑</span>)
        - GPS受信確認用LED(<span style="color: blue;">青</span>)


> PIN1 : UART接続確認用LED
> 
> PIN2 : GPS受信確認用LED
> 
> ![LED](Manual_Telematar_LED.JPG "LEDの接続")
>
***

3. アンテナの接続
   - 用意するもの
     - MKR WAN 1310用ダイポールアンテナ × 1


> MKR WAN 1310へダイポールアンテナを写真の箇所へ接続する。
> 
> ![ダイポールアンテナ](Manual_Telematar_Antenna.JPG "ダイポールアンテナの接続")
>  

4. MKR GPS Shieldの接続
   - 用意するもの
     - MKR GPS Shield用ケーブル × 1


> MKR WAN 1310とMKR GPS Shield用ケーブルが接続されていない場合にこの手順を踏む。
>
> 青色の円で囲まれた箇所に接続する。
>
> 上下の向きがあるので気をつけて接続する。
>
> うまく入らない時は落ち着いて向きを確認すること。
> 
> ![GPS](Manual_Telematar_GPS.JPG "MKR GPS ShieldとMKR WAN 1310の接続") 

5. 電源ポートの接続
   - 用意するもの
     - ピン数2のコネクタ付きケーブル × 1


> 5Vのシルクがある箇所に接続する
> 
> <span style="color: red; background-color: yellow;">!!! 注意 !!! </span>
>
> <span style="color: black; background-color: yellow;">!!! 電源基板への接続はまだ行わないこと !!!</span>
> 
> ![電源接続](Manual_Telematar_Power.JPG "電源接続")
>
***

6. 計器タワーの組み立て
   - 用意するもの
     - M4×25mmのスペーサー 
       - 1基板当たり × 4

> 右上に黒色の印を向け組み立てる
> ![組み立て](Manual_Telematar_Kumitate.JPG "計器タワーの組み立て")
>
***

7. モジュールに電源を入れる
    - 用意するもの
      - 「6. 計器タワーの組み立て」でその他基板も組み合わせた計器タワー

> 電源基板の **5.0V** 出力ピンに「5. 電源ポートの接続」で接続したケーブルを挿入する。
> 
> （写真では分かりやすくするためにタワーを組んでいない状態となっている。）
>
> !!! 注意 !!!
> 
> !!! **12.0, 9.0V出力ピンへ挿入しないこと** !!!
> 
> ![電源接続](Manual_Telematar_PowerConnect.JPG "電源接続")
>
***



