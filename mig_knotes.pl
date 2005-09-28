#! /usr/bin/perl

# mig_knotes.pl
# Dieses Programm erzeugt aus einem notes.ics- file (KDE >3)
# pro Note eine Datei (KDE <3).
#
# Der ics- File beinhaltet die Abschnitte "Description" (=Notizen)
# und "Summary" (=Titel der Note)
#
# Es wird pro Note ein file .KNotes *n*_data (Notizen) sowie
# KNote*n* (Steuerdatei) erzeugt.
#
# Das Programm ist "auf die Schnelle" geschrieben. 
# Es ist nicht empfehlenswert -w oder :use_strict einzuschalten.
# 
# Wer es benutzt und Verbesserungen vornimmt, kann mir freundlicherweise 
# eine Kopie schicken.
#
# chaeffne@web.de, Mai 2005
#
#
#
# Folgende Pfade muessen individuell gesetzt werden:

$imp = "/pfad/zum/importfile/notes.ics"; # Importfile
$verz = "/pfad/zum/export-verzeichnis/tmp"; # Exportverzeichnis
$steuerdatei = "/pfad/zu/einer/steuerdatei/KNote"; # KNotes- Steuerdatei KDE <3;
						   # wird als Vorlage fuer 
						   # die neuen verwendet.
####################################
# Beginn des Programms #############
####################################
#
# 1. Abschnitt
# In diesem Abschnitt werden die "Descriptions" in notes.ics gezaehlt (=Dateianzahl), 
# die Anfangs- und Endzeile der Description bestimmt, sowie der Titel der 
# Notes (=Summary) extrahiert.
open (IMP,"$imp");
$zz = 1; #Zeilenzaehler
$fc = 1; #Filecounter
while ($_ = <IMP>){

$all{$zz} = $_; # alle Zeilen in ein assoziatives Array schreiben

    if ($_ =~ /^DESCRIPTION/){ # Nach "Description" am Anfang suchen
    $anfang{$fc} = $zz; # Anfang: Ein Wertepaearchen aus filecounter und Zeilenzaehler erzeugen
    }
    elsif ($_ =~ /^SUMMARY/){ # Nach Summary am Anfang suchen
    $ende{$fc} = $zz-1; # Ende: Ein Wertepaearchen aus filecounter und Zeilenzaehler erzeugen
    $ttt = $_; # Titel
    $ttt =~ s/SUMMARY://;
    $title{$fc} = $ttt; # Den aktuellen Titel mit dem filecounter assozieren
    $fc++;
    }
$zz++;
} # Ende 1. Abschnitt
close(IMP);

# 2. Abschnitt
#
# In diesem Abschnitt werden die Files erzeugt und die oben gewonnenen Daten verarbeitet.
$fc2 = 1; # filecounter2
while ($fc2 <= $fc) {
    
    # Datendatei (= Notiz)
    $filename = "$verz/.KNote ".$fc2."_data";
    open(EXP,">$filename");
    
    $zz = 0;
    $anzahl = $ende{$fc2}-$anfang{$fc2}; # Anzahl der Zeilen
    while ($zz <= $anzahl) { # fuer alle betroffenen Zeilen durchlaufen
    $line = $all{$anfang{$fc2}+$zz}; # die aktuelle Zeile
    $line =~ chop($line); # letztes Zeichen (=new line) abschneiden 
    $line =~ s/DESCRIPTION://; # das Wort rausschneiden
    @line = split(/\\n/,$line); # die Zeile nach new lines teilen (=Steuerzeichen im ics-file)
    $anz = @line; # die Anzahl im array
    
	$zz2 = 1;
	foreach $nl(@line) { # fuer jeden Wert im Array
	    $nl =~ s/\\t/  /g;# Tabs durch Leerzeichen ersetzen
	    $nl =~ s/\\//g;# \ (als Escape verwendet zB. vor ",;") entfernen
	    # eine neue Zeile?	    
	    if ( $zz2 < $anz or $anz == 1 ) {
	    print EXP "$nl<br>";
	    }
	    else {
	    print EXP $nl;
	    }
	$zz2++;
	}
	    
    $zz++;
    }

    # Steuerdatei
    $filename2 = "$verz/KNote ".$fc2;
    open (IMP2,"$steuerdatei"); 
    # eine Beispieldatei, die fuer alle Notes verwendet wird.
    open(EXP2,">$filename2");
    while ($_ = <IMP2>) {
    if ($_ =~ /^name/) {
    print EXP2 "name=$title{$fc2}\n";
    }
    else{
    print EXP2 $_;
    }
    }
$fc2++;
} # Ende 2. Abschnitt
close(EXP);
close(EXP2);
close(IMP2);
# Ende mig_knotes.pl














