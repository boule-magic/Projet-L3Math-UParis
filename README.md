# Projet-L3Math-UParis
***
Compilation : 
```
gcc -Wall main.c conversion.c palette.c pngio.c -lpng -O2
```
Exécution par défaut (équivalent) : 
```
./a.out img/taric.png
./a.out img/taric.png img/output.png
```
***
Options :
 * p : Choix de la palette de couleurs
 * f : Choix de l'algorithme Floyd-Steinberg
***
Arguments de l'options "-p" :
  * 8 : Palette de 8 couleurs : saturation
  * 16 : Palette de 16 couleurs : CGA
  * 64 : Palette de 64 couleurs : 4-4-4
  * 216 : Palette de 216 couleurs : 6-6-6
  * 252 : Palette de 252 couleurs : 6-7-6
  * 2 : Palette de 2 couleurs : noir et blanc
  * 256 : Palette de 256 couleurs : niveaux de gris
***
Exemple d'utilisation d'options :
```
./a.out img/taric.png -p 8 -f
```
***
Convertir des .jpg en .png (writergba)
* Le rendre exécutable :
```
sudo chmod +x ./writergba
```
* Utilisation
```
./writergba img/taric.jpg img/taric.png
```
***
Bonus (valgrind)
* Installation de valgrind
```
sudo apt install valgrind
```
* Compilation (valgrind) : 
```
gcc -Wall main.c conversion.c palette.c pngio.c -lpng -g
```
* Exécution (valgrind) : 
```
valgrind ./a.out img/taric.png
```
