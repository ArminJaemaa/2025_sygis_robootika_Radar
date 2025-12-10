# Projekti dokumentatsioon

## 1. Projekti eesmärk ja seadme lühikirjeldus
**Mis asi see on, mida ja miks me teeme? Millist praktilist probleemi see lahendab?**

- Meie projekti eesmärk on luua radar, mis tuvastab infrapuna sensoriga kuni 150 cm kaugusel olevaid objekte 160 kraadises vaatlusnurgas.
- Süsteem eraldiseisvana mingit olulist probleemi ei lahenda, kuid kui seda integreerida mõne muu süsteemiga, siis võib ta lahendada mitmeid probleeme. Seadet saab kasutada näiteks autonoomsete seadmetel kokkupõrgete vältimiseks muude objektidega.
- Peamised komponendid: servo mootor, infrapuna sensor, tft ekraan, karp (3d prinditud), Arduino.

---

## 2. Sisendite loetelu
**Millised on süsteemi poolt loetavad / mõõdetavad sisendid? Millega neid mõõdetakse / tuvastatakse?**

- Infapuna sensor - Mõõdab pinget ja Arduino arvutab selle põhjal kauguse.

---

## 3. Väljundite loetelu
**Mida süsteem teeb / muudab? Millega väljund realiseeritakse?**

- Arduino arvutab vastavalt infrapuna pingele kauguse ja kuvab ekraanil vastavalt arvutatud kaugusele ja servo mootori hetkenurgale objekti punase täpi kujul.

---

## 4. Nõuded loodavale seadmele
**Mis peab toimuma, kui kasutaja teeb mingi toimingu? Kirjelda käitumisloogika.**

- Kuna tegemist ei ole otseselt interaktiivse süsteemiga, siis peale seadme sisse lülitamist on kasutaja ülesanne jälgida ekraanil toimuvat ja käituda vastavalt.
- Süsteemi käivitudes peab mootor liikuma algusesse ning ekraanile joonistuma esmane radari kujutis. Misjärel käivitub järgmine tsükkel:
- 1) Servot liigutatakse 5 kraadi võrra edasi; 2) Mõõdetakse n korda (hetkel 10) kaugust selle nurga peal; 3) Filtreeritakse mõõtmisvead välja ning sobilike tulemustega arvutatakse keskmine ning kontrollitakse, et see jääks mediaanist +/-20% sisse; 4) Kustutatakse vana "Sweep line" ning sellel nurgal varem eksisteerinud täpp ja joonistatake liigutud nurga peale uus Sweep line ja uus täpp; 5) Saadud kauguse ja hetkelise servo nurga abil joonistatakse ekraanil radari alusele vastavasse kohta punane täpp;
- Kui jõutakse 170 kraadi peale, pööratakse suund ringi ning alustatakse sama tsükli kordamist teises suunas kuni jõutakse 10 kraadini ning pööratakse taaskord ümber ja jätkatakse tegevust kuni toide kaob või esineb mingi süsteemi lõhkuv viga. 

---

## 5. Süsteemi füüsiliste komponentide loetelu
**Millest seade koosneb? Lisa lingid või täpsed nimed, et keegi teine saaks sama asja uuesti osta / teha.**

- Arduino Uno R3
- SHARP GP2Y0A02YK0F IR-distantssensor - Mõõdab vahemaad 20–150 cm täpsusega
- DM996 Servo mootor - Pöörab sensorit 160° ulatuses
- 2.8” SPI ILI9341 TFT ekraan - Kuvab radaripildi ja täppe
- 2x 3V to 5V Level converter 2PH240074A
- USB-toide arvutist - Toidab kogu süsteemi
- 3D-prinditud ”maja” - Hoiab servo + sensori kindlas asendis, tagab stabiilse mõõtmise

---

## 6. Ühendusskeem
**Kuidas kõik osad on omavahel ühendatud?**

![Ühendusskeem](https://github.com/ArminJaemaa/2025_sygis_robootika_Radar/blob/main/media/image.png)

- TFT ekraani ühendused, kuna skeemil on neid täpseid pin-e näha ainult pin descriptionil Fritzingus
- Pin 8 -> TFT CS pin
- Pin 9 -> TFT RST pin
- Pin 10 -> TFT D/C pin
- Pin 11 -> TFT MOSI pin
- pin 13 -> TFT SCK pin
- 5V -> TFT VCC pin
- GND -> TFT GND pin

---

## 7. Süsteemi juhtiv kood (või pseudokood)
**Kirjelda programmi loogikat nii, et seda on võimalik aru saada ka hiljem.**  

[Koodi fail /src kaustas](https://github.com/ArminJaemaa/2025_sygis_robootika_Radar/blob/main/src/teinePrototyypEkraaniga/teinePrototyypEkraaniga.ino)
Kood on kirjeldatud võimalikult täpselt kommentaaridega
- 1) Servot liigutatakse 5 kraadi võrra edasi; 2) Mõõdetakse n korda (hetkel 10) kaugust selle nurga peal; 3) Filtreeritakse mõõtmisvead välja ning sobilike tulemustega arvutatakse keskmine ning kontrollitakse, et see jääks mediaanist +/-20% sisse; 4) Kustutatakse vana "Sweep line" ning sellel nurgal varem eksisteerinud täpp ja joonistatake liigutud nurga peale uus Sweep line ja uus täpp; 5) Saadud kauguse ja hetkelise servo nurga abil joonistatakse ekraanil radari alusele vastavasse kohta punane täpp;
- Kui jõutakse 170 kraadi peale, pööratakse suund ringi ning alustatakse sama tsükli kordamist teises suunas kuni jõutakse 10 kraadini ning pööratakse taaskord ümber ja jätkatakse tegevust kuni toide kaob või esineb mingi süsteemi lõhkuv viga. 

