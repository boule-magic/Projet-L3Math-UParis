# Projet-L3Math-UParis
***
* Compilation : 
```
gcc -Wall main.c pngio.c -lpng -O2
```
* Exécution par défaut : 
```
./a.out img/taric.png
./a.out img/taric.png img/output.png
```
***
* Options :
  * a : Palette de 8 couleurs : saturation
  * b : Palette de 16 couleurs : CGA
  * c : Palette de 64 couleurs : 4-4-4
  * d : Palette de 216 couleurs : 6-6-6
  * e : Palette de 252 couleurs : 6-7-6
  * f : Palette de 2 couleurs : noir et blanc
  * g : Palette de 256 couleurs : niveaux de gris
***
* Exemple d'utilisation d'options :
```
./a.out -e img/taric.png
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
gcc -Wall main.c pngio.c -lpng -g
```
* Exécution (valgrind) : 
```
valgrind ./a.out img/taric.png img/output.png
```
