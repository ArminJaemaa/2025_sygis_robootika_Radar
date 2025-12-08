# Projekti dokumentatsioon

## 1. Projekti eesmärk ja seadme lühikirjeldus
**Mis asi see on, mida ja miks me teeme? Millist praktilist probleemi see lahendab?**

- Meie projekti eesmärk on luua radar, mis tuvastab infrapuna sensoriga kuni 150 cm kaugusel olevaid objekte 160 kraadises vaatlusnurgas.
- Süsteem eraldiseisvana mingit olulist probleemi ei lahenda, kuid kui seda integreerida mõne muu süsteemiga, siis võib ta lahendada mitmeid probleeme. Seadet saab kasutada näiteks autonoomsete seadmetel kokkupõrgete vältimiseks muude objektidega.
- Peamised komponendid: servo mootor, infrapuna sensor, tft ekraan, karp (3d prinditud), Arduino.

---

## 2. Sisendite loetelu
**Millised on süsteemi poolt loetavad / mõõdetavad sisendid? Millega neid mõõdetakse / tuvastatakse?**

- Infapuna sensor
Mõõdab pinget ja Arduino arvutab selle põhjal kauguse.

---

## 3. Väljundite loetelu
**Mida süsteem teeb / muudab? Millega väljund realiseeritakse?**

Arduino arvutab vastavalt IR pingele kauguse ja kuvab ekraanil vastavalt arvutatud kaugusele ja servo mootori hetkenurgale objekti punase täpi kujul.

---

## 4. Nõuded loodavale seadmele
**Mis peab toimuma, kui kasutaja teeb mingi toimingu? Kirjelda käitumisloogika.**

Kuna tegemist ei ole otseselt interaktiivse süsteemiga, siis peale seadme sisse lülitamist on kasutaja ülesanne jälgida ekraanil toimuvat ja käituda vastavalt.

---

## 5. Süsteemi füüsiliste komponentide loetelu
**Millest seade koosneb? Lisa lingid või täpsed nimed, et keegi teine saaks sama asja uuesti osta / teha.**

Tabelina või punktidena. Nt:

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

---

## 7. Süsteemi juhtiv kood (või pseudokood)
**Kirjelda programmi loogikat nii, et seda on võimalik aru saada ka hiljem.**  
Kui kood töötab, pane siia lühike selgitus + viide failile `src/projektinimi.ino`.  
Kui kood pole veel valmis, lisa siia pseudokood.
