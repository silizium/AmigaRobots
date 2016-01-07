##AmigaRobots

7.1.2016

A clone of PCRobots for PC where several robots fight each another in an arena where they try to destroy the other robots. It is possible to fight in teams and the original program was first published in the M.A.G. Mailbox in Hamburg/Germany on Christmas eve 1992. The last published version was from 7.6.1995. It was also published (without sourcecode) on #206 TimeDisk and as far as I remember.

The whole documentation is in German and the code is most likely an ugly hack. For it never was intended to be published and it's one of my first programs in C anyways. So cat "comments about style" >/dev/null 
If you like it and you manage to restart it/recompile it, please share it and fork/push a merge request. I won't work on this project anymore and I have forgotten how I did it. But I'll share it for fun. And for the good ol' times.

Intended to run on the Amiga Computer. 

#Programmentwicklung

- Betareleases vom 24.12.92 bis 29.1.93
unver�ffentlicht, nur f�r Beta-Tester
- Version 1.0.1 - 29.1.93
erste Version in der MAG Hamburg
- Version 1.0.11 - 1.2.93
ge�nderter   Infotext,  die  Zwischenversionen  sind  f�r  den
Versuch  draufgegangen,  die langsamen Explosionen nebenl�ufig zu
programmieren.   Da  der  Spa�  nichts  bringt  und sowieso nicht
wirklich  stabil  lief,  habe  ich es wieder normal programmiert.
Sven   wird   sich   sicherlich   �rgern...   Au�erdem  ein  paar
Sch�nheitsfehler  beseitigt,  die aber sicher niemandem au�er mir
auffallen werden.
- erste Ver�ffentlichung im Fras-Netz
- Ver�ffentlichung auf TIME 206
- Version 1.0.23 23-Jul-93
-- Bug bei nicht vorhandener req.library beseitigt
-- Alle Fonts sind auf Topaz80 festgenagelt.  Bei Nutzung eines
alternativen  Defaultfonts  unter  KS/WB  2.0 war die Darstellung
total  vermackelt.   Leider hatte ich noch keine Zeit, einen frei
w�hlbaren  Font  einzubauen.  Daf�r m��te ich die gesamte Grafik-
und  Men�struktur  neuschreiben,  da ich in einem Anflug von Wahn
eine abartige Men�/Grafiktoolbox benutzt habe.
-- Update der Real- und Usenet-Anschrift
-- die Kommandozeilenoption -f hat sich ge�ndert auf -o, da in
Zukunft -f f�r den Font reserviert ist.   
-- Dokumentation vollst�ndig umgestellt, Archiv beinhaltet
jetzt texi,dvi,dok und guide - Dokumentationen
-- Version 1.1 Programm, Version 1.3 der Dokumentation
-- alle robotlib-Includefiles kompatibler f�r andere Compiler
gemacht
-- alle Roboter @code{_ProgramName}-Statements verpa�t und in
@code{#ifndef __SASC} eingeschlossen und den ganzen Kram neu durchkompiliert.
-- weitere Erweiterung und Verbesserung der Dokumentation, z.B.
Dice/Gnu-C++-Anhang und 1000 Kleinigkeiten (keine �bertreibung)
(@pxref{Fragen und Antworten}).
-- Sch�sse werden nur noch von W�nden aufgehalten, anders als
bei PCRobots.  Dort werden Sch�sse auch von den anderen
Hindernissen aufgehalten.  Das original PCRobots-Verhalten kann
man jetzt mit der neuen Option -c aktivieren (@pxref{-c Option}).
-- Bug beseitigt, der manchmal zu falscher Parameter�bergabe
bei shoot f�hrte und dort seltsame Effekte erzeugte.
-- Der Men�punkt "Entfernen" erm�glicht das Entfernen von
schadhaften Robotern. 
-- Linkfiles f�r Dice und Gnu-C++ liegen bei
-- Neue Option -t Zeitfaktor, WB-Tooltype TIMEFACTOR, gibt die
Schrittweite der Simulation an, so da� man die Simulation
beschleunigen kann. Da dadurch eine Ungenauigkeit entsteht, ist
diese Option als "Draftmodus" zu verstehen (@pxref{-t Option}).
-- Das Konfigurationswindow wurde um das Zeitfaktorgadget
erweitert
-- Das Statusfenster schaltet sich nun automatisch inaktiv,
damit die Men�s sofort weiter erreichbar sind.
-- In der Icons-Directory liegen ein paar nette Icons f�r
AmigaRobots von Oliver Clouth. Ich konnte mich noch nicht
f�r eins entscheiden, weil die Dinger erst gestern
angekommen sind.
-- 8.9.93 Ver�ffentlichung im FRAS-Netz
- Version 1.2
-- 10.9.93 neue -i Option, neuer MONITORID-Tooltype. Dokumentation
dieser Features. Damit ist es nun m�glich AmigaRobots auf
jedem beliebigen Bildschirmmodus zu starten. Diese Option
ist aber nur auf Kickstart 2.0 oder h�her aktiviert (@pxref{-i
Option}).
-- PubName `AmigaRobots' unter 2.0. Der AmigaRobots-Bildschirm
ist public (@ref{Systemanforderungen} und @ref{Fragen und Antworten}).
-- Ausbau der Fragen und Antworten, der Einleitung und Hinzunahme
des Geschichte-Anhangs, damit man direkt an die neuen Stellen
bl�ttern kann. 
@xref{Fragen und Antworten}, @ref{Einleitung}.
-- 11.9.93 Entfernte Roboter werden durchgestrichen
-- 8.11.93 Statusfenster zeigt x/y Postition an
-- Die voreingestellte Schu�zahl wurde auf 40 gesenkt und bei 
Tarnvorrichtung nicht mehr 9/10 sondern 1/2 der Munition
einbehalten.
-- @xref{-m Option}, @ref{-c Option}, @ref{Konfiguration} und @ref{shoot}.
-- flying_shells()-Funktion hinzugef�gt
Diese Funktion ist nicht mehr PCRobots-kompatibel.
@xref{flying_shells}.
-- get_big_map()-Funktion hinzugef�gt
Diese Funktion ist nicht mehr PCRobots-kompatibel.
@xref{get_big_map}.
-- hit_from()-Funktion hinzugef�gt
Diese Funktion ist nicht mehr PCRobots-kompatibel.
@xref{hit_from}.
-- AmigaRobots_guide.ced-Script dem Archiv beigelegt
Mit diesem Script ist es m�glich, vom CygnusED aus die
AmigaRobots-Guide-Datei als Onlinehilfe zu benutzen. Es wird
automatisch der Befehl aufgerufen, auf dem der Cursor steht.
Anpassungen der Pfade sind allerdings von Hand vorzunehmen.
-- Quadratischer Energiehaushalt f�r die Geschwindigkeit
Der Energiebedarf f�r eine Strecke ist geringer bei kleinerer
Geschwindigkeit. Dadurch entsteht ein realistischer
Energiehaushalt. Ein Roboter mit einer Geschwindigkeit von 63
verbraucht jetzt soviel Energie, wie die Generatoren aufladen
k�nnen. Dadurch sind die Roboter insgesamt beweglicher geworden
und die Simulation komplexer.
@xref{Batterien}, @ref{-c Option}.
-- Zus�tzliche Energiekosten beim Beschleunigen
Dadurch werden Schummeleffekte mit der Energie/Lenkbarkeit verhindert. Die
einzige L�sung f�r das Problem war mehr Realismus. Diese Funktion
ist nicht abschaltbar.
@xref{Batterien}.
-- 23.12.93 korrekte Glue-Datei f�r DICE beigelegt 
Au�erdem wurde die Herstellungsanweisung korrigiert. Unter 1.3
m��te die Datei dann aber trotzdem neu hergestellt werden.
@xref{Fragen und Antworten}.
- Version 1.3
-- 26.6.95 hinzuf�gen der Funktionen l_asin und l_acos
Diese beiden Funktionen wurden vermi�t und deshalb nachtr�glich
implementiert. Diese Funktionen sind nicht PC-Robots kompatibel.
@xref{l_asin}, @ref{l_acos}.
-- 6.7.95 hinzuf�gen der Funktion highest_player_id(), myid() und
version(). Wechsel des rechtlichen Status von Giftware auf
Fairware. Ab sofort voller Support auch f�r nicht registrierte
Benutzer. �berweisungen von Geld werden trotzdem gern gesehen. Im
Paket liegt au�erdem der Roboter Nexus.rob bei, der nebenbei den
ersten Platz abgesahnt hat.
@xref{highest_player_id}, @ref{myid}, @ref{version} und 
@ref{Kontaktadresse und Registrierung}

#Bugs

- Kugeln am linken Rand verschwinden nicht.
Unerkl�rlich. Ich mu� dar�ber mit meinem Meditationsbrahmanen
reden.
- Die Berta hat aufgeh�rt zu schie�en, obwohl noch ein Gegner
vorhanden war. Bug in Berta oder Robots?
- unerkl�rlich ist auch, warum auf au�ergew�hnlich schnellen
(40 MHz) 68040-Amigas die Roboter manchmal "Spuren" hinterlassen.
Sollte eigentlich nicht passieren. Vielleicht ein Fehler in der
graphics.library?

#todo

- vern�nftige Statistikauswertung
- Arena-Editor
- Au�ergew�hnliches Icon, am besten in 8 "MagicWB"-Farben und
Hochaufl�sung, d. h. 1/1 Pixelverh�ltnis
- Fertigstellung meines neuesten Robterprojektes "G�nger"
