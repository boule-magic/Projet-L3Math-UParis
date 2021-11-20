# Projet-L3Math-UParis
***
* Compilation : 
```
gcc -Wall main.c pngio.c -lpng -O2
```
* Exécution : 
```
./a.out img/taric.png
./a.out img/taric.png img/output.png
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
