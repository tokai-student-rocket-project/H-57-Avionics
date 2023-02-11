/*
H-57

機体内部及び機体外部から電源を供給できなくなった場合に使用します。
*/

#include "VarSpeedServo.h"

VarSpeedServo Supplyservo;

int SupplyServoPin = 6;

float Supplyservo_deg;

void setup()
{
    Serial.begin(9600);

    Supplyservo.attach(SupplyServoPin);
    Supplyservo.write(20, 30, true);
}

void loop()
{

}

/*
                                      .[
                                   .dMM@
                   ...&gNMMMMNNgJ+MMMMMN
               ..MMMMH"""7??77""MMMMMMMM;
            .JMMMMMMMMMMMMNg,. .MMMMMMMMN,
          .MMMWMMMMMMMMMMMMMMMNMMMMMMMM"WMN,
        .dM#^JM#"         ?TMMMMMMMMMM^  .WMN,
       .MMt JM^              .MMMMMMM'     ,MMb
      .MM! .@               .MMMMMMMMb       UMN.
     .M#`  d`               dMMMMMMMMMN,      UMN
    .MM!   F               .MMMMM#`,MMMM,      HMb
    MMF    \    ...       .MMMMM#`  .MMMM,     ,MM.
   .MM`    i.MMMMMMMMNJ.  dMMMM#     .MMMN.     MM]
   -M#   .dMMMMMMMMMMMMMN.MMMM@       ,MMMb     JM@
   (M#  .MMMMMMMMMMMMMMM# TMM@         dMMM;    (M#
   -M# .MMMMMMMMMMMMMMMM'  WF          .MMMb    JMF
   .MM.dMMMMMMMMMMMMMMMF   .            MMMN    MM]
    dMbMMMMMMMMMMMMMMM@   .#            MMMN   .MM
    .MMdMMMMMMMMMMMMMM!  .MF            MMM#  .MMt
     -MMMMMMMMMMMMMMMF  .M@            .MMMF .MMF
      (MMMMMMMMMMMMM@  .MD            .MMMM`.MMF
       ,MMNTMMMMMMMM! ,"             .MMMM\.MM3
         TMMa._7"""QN"(.           ..MMMMQMM@`
           TMMN,  .M3   74J......(MMMMMMMM@!
             ?WMMNML.       ?"""HHMNMMM#"
                (MHMMMMNggg&ggNMMMMM9^
                .^   -?""""""""7!
*/
