/*
  Writer?:Hiroki TSUTSUMI(1CES2116)
  Rocket:H-57
  MCU Series:Arduino
  MCU:SAMD21

  GPS,バルブ位置を機体のアビオニクスから受信する機能を持っています。
*/

#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> packet; //課題 > packetのサイズの最適化

void setup()
{
  Serial.begin(115200);
  LoRa.begin(921.8E6); //ARIB Unit Channel Number 30 を採用する。
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{

  if (Serial.available() > 0)
  {
    LoRa.beginPacket();
    LoRa.print(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }

  if (LoRa.parsePacket())
  {
    deserializeJson(packet, LoRa);
    char output[1024];
    serializeJson(packet, output);
    
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(output);
    
    /*
    Groundの状況に応じて実装

    Serial.print(" || RSSI: "); // 通信強度がわかります。
    Serial.println(LoRa.packetRssi());
    Serial.println();
    */
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

/*
                                                   .,
                                                .&MMb
                                             .(MMMMMN
                         ..JgNMMMMMMMMMMNNg(MMMMMMMMM-
                    ..gMMMMMMMMMH"""""MMMMMMMMMMMMMMM]
                 .gMMMMMMNg+&+J...       .MMMMMMMMMMM@
              .gMMMMMMMMMMMMMMMMMMMMNJ.  dMMMMMMMMMMMMN,
            .MMMM8gMMMMMMMMMMMMMMMMMMMMMNMMMMMMMMMMMTMMMMa.
          .MMMM=.MMMM9"`       _7"MMMMMMMMMMMMMMMMM^  ,WMMMe
        .dMMM= JMM@!                ?TMMMMMMMMMMMM'     .WMMN,
       .MMMF  JM@`                    .MMMMMMMMMM'        /MMMb    `
      .MMM^  .MF                     .MMMMMMMMMMMx          WMMN.
     .MMM!   dF                      dMMMMMMMMMMMMN.         UMMN.
    .MMM`   .#                      .MMMMMMMM#MMMMMN,         UMMb
   .MMM^    ,]       `  `  `  `  ` .MMMMMMMM#  WMMMMMe         MMM]
   dMMF     ,!                     MMMMMMMM@    TMMMMM[  `     ,MMM.
  .MMM      ,    ...(JJ-...       -MMMMMMM@      ?MMMMMc        dMMb
  (MMF      .(.MMMMMMMMMMMMMN,   .MMMMMMMF        qMMMMM,   `   ,MMN
  dMM%     .MMMMMMMMMMMMMMMMMMMa.MMMMMMMF          WMMMMN        MMM_
  MMM}   .dMMMMMMMMMMMMMMMMMMMMM$TMMMMMF           .MMMMM[      `MMM}
  MMM{  .MMMMMMMMMMMMMMMMMMMMMMF  ?MMMF             JMMMMN       MMM}
  MMM] .MMMMMMMMMMMMMMMMMMMMMMM`   dMD        `     .MMMMM;      MMM~
  JMMb -MMMMMMMMMMMMMMMMMMMMMM%    .$                MMMMM]     .MM#
  .MMN.dMMMMMMMMMMMMMMMMMMMMMF     (.                dMMMMb     JMMF
   WMMbdMMMMMMMMMMMMMMMMMMMMM`    JM`                dMMMM@    .MMM`
   ,MMMJMMMMMMMMMMMMMMMMMMMM%    JM#              `  dMMMMF    dMMF
    JMMNMMMMMMMMMMMMMMMMMMMF    dMM^          `      MMMMM]   JMM@
     ?MMNMMMMMMMMMMMMMMMMM#`  .dMM3                 .MMMMM`  JMM#
      ?MMNMMMMMMMMMMMMMMMM\  .dM#'                 .MMMMMF .dMM@
       (MMMmTMMMMMMMMMMMMF  .M#^                  .MMMMM@ .MMMF
        .WMMN,?TMMMMMMMM#  .7                    .MMMMM@.dMMM^
          /MMMN,    ???~dM#_4,.                .MMMMMMNdMMMD
            ?MMMMa.    .M@    (YQ...       ..gMMMMMMMMMMMD
              (WMMMNa,.M@         7"MMMMMMMMMMMMMMMMMMM"
                 ?WMMMMMg...            ??7uagMMMMMM"`
                    -WMMMMMMMMNNgggggggNMMMMMMMM9^
                    .F   _7"WMMMMMMMMMMMMH""^
                    r
                                                                   `
*/