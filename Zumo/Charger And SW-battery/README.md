# Eksempel på IR-basert ladestasjon-kode for Zumo prosjekt.
Et eksempel på IR-basert ladestasjon og SW-batterimodul

Koden benytter sensor: IRM-56384 IR mottaker (dokumentasjon: https://datasheet.lcsc.com/lcsc/2201242300_Everlight-Elec-IRM-56384_C72048.pdf)

Koden er modifisert versjon av eksisterende prosjektkode:

# Arduino
Grunnleggende prinsipp for ladestasjonen er at den konstant sender ut beskjeder om at dette er en ladestasjon som en mottaker kan ta imot.
Når en mottaker får beskjeden sender den et svar basert på hva mottakerens ønsker.
Om mottakeren trenger oppladning sender den et oppladningsforespørselssignal til ladestajonen for hver enhet den ønsker å lade, 
som bruker dette til å huske hvor mye mottakeren har ladet totalt. 
Når mottakeren er ferdig sender den et signal som ladestasjonen bruker til å legge den totale oppladningen til et arrayelement som tilhører mottakerens ID.

# Zumo
Inneholder to addisjoner til koden: SW-batteri og mottakerkode for ladestasjonen.
SW-Batteriet er en enkel funksjon som tapper batteriet basert på makshastighet.
Mottakerkoden tar imot ladestasjonens kode og setter zumoen i lade-modus når den får signal.
Når den er i lademodus sender den oppladningsforespørsel fram til den er oppladet. 

