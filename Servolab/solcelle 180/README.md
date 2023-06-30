# Servolab solcellepanel
Dette er et forslag til laboppgave om et solcellepanel som flytter seg selv ut ifra lysstyrke.
Det er to versjoner en som styres av [PWM](https://github.com/WexTheX/sommerjobb-2023/blob/main/Servolab/solcelle%20180/solcelle_PWM/solcelle_PWM.ino) og en som styres av [servo biblioteket](https://github.com/WexTheX/sommerjobb-2023/blob/main/Servolab/solcelle%20180/solcelle_bibliotek/solcelle_bibliotek.ino).
Koden fungerer ved at man bruker tre fotoresistorer til å måle lysstryken fra tre forskjellige vinkler:
En som peker mot høyre, en som står midt på og en som peker mot venstre.
Disse sammenlignes for å finne ut hvor lystyrken er sterkest. Om den er sterkest midt på står den i ro, mens om den er sterkest mot en annen side vil den forflytte seg.
Vi benytter en 3D printet del for å lage skillevegger mellom fotoresistorene.

Forskjellen på PWM versjonen og biblitek versjonen er minimal, hovedsakelig at biblitek-versjonen tillater bruk av grader

# Oppkobling:

# Deler:
## Servo:
Vi benytter oss av servo SM-S2309S [http://descargas.cetronic.es/microservo.pdf]

## 3D-printet del:
