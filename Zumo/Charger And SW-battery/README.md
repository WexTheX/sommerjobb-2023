# Eksempel på IR-basert ladestasjon-kode for Zumo prosjekt.
Et eksempel på IR-basert ladestasjon og SW-batterimodul

Koden benytter sensor: IRM-56384 IR mottaker (dokumentasjon: https://datasheet.lcsc.com/lcsc/2201242300_Everlight-Elec-IRM-56384_C72048.pdf)

Koden er modifisert versjon av eksisterende prosjektkode:

# Ladestasjon
I charger_arduino filen ligger koden for ladestasjonen. Konseptet er at den konstant sender ut en beskjed om at den er en ladestasjon og venter på svar.
  Når ladestasjonen får svar fra en robot begynner den dialog om oppladning hvor roboten ber om hvor mye den skal lades opp.
  Når roboten er fulladet mottar ladestasjonen en beskjed om dette og lagrer mengden ladet i en array som ligger i minnet.
  I tillegg sender ladestasjonen ut en beskjed til roboten om at den kan kjøre videre, for å bekrefte at den har lagret orderen.
  Ladestasjonen har også en knapp som man kan bruke til å manuelt overstyre den automatiske ladefunksjonen på roboten og initere en ladning neste gang roboten passerer.


# Zumo
I charger_zumo filen ligger kode for SW-batteriet og opplading av dette. 
  SW-batteriet har to moduser "draining" og "charging". 
  
  Under draining blir batteriet tappet ut ifra rotasjonen på hjulene til roboten ved bruk av encoderfunksjonen getCountAndReset() som den kjører hvert sekund. 
  Batteriet har en kapasitet på 255. Når den passerer 40% av makskapasiteten vil den automatisk be om å lade ved neste passering av ladestasjonen. 
  Når den passerer 10% av makskapasiteten vil roboten sakke ned veldig og kjøre på sparemodus.
  
  Under charging vil den stoppe når den får vite at den er ved en ladestasjon og initiere ladning.
  Når den lader vil den sende en beskjed for hver enhet den lader.
  Når den er ferdig med å lade vil den sende et signal om at den er ferdig. Ladestasjonen vil sende en bekreftelse som tillater roboten å kjøre videre.
