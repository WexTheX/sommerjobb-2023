# LED-planke programmer
Dette er en under-repo for programmer som kan kjøres på LED-plankene.

## BIN-calc
BIN-Calc er et program som visuelt representerer hvordan addisjon og subtraksjon av to nummer gjøres med binærtall.
Koble opp tre ledstriper på rad. La disse helst stå opp ned ettersom dette vil gjøre at tallene vises fra høyre til venstre.
Grunnen til dette er fordi binærtallenes verdi og indexsen til LED-plankene ikke går bra sammen om de ligger rett vei. Å snu de opp ned gir best representasjon på LED-planke V1.

Programmet tar inn et uttrykk i Serial, for eksempel "101 + 202" eller "10000000-20000". 
Det tar ikke hensyn til mer enn to tall eller andre tegn enda.

## GRAY-Code
Gray-Code er et program som visuelt representerer Gray kode på LED-planken.
Den kobles opp mot en LED-planke og en knapp.
Når knappen trykkes øker den tallet med 1.
I koden blir tallet kovertert til sin Gray code version.
Man kan kommentere ut denne biten for å heller se binærtall.
LED-planken burde snues opp ned for best representasjon pga samme grunn som hos BIN-calc.
