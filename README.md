# Projet-L3Math-UParis

Il s'agit d'un petit programme de compression d'image qui réduit une image PNG en une image PNG à palette de couleur choisi. Pour un meilleur rendu il se sert des algorithmes de diffusion d'erreur de Floyd-Steinberg, d'Atkinson et ordonné. Pour une meilleur compression il peut également réduire la taille de l'image.

## Compilation
#### Setup : 
```
sudo apt install gcc make
```
#### Compiler : 
```
make
```
ou
```
make compress
```
## Tester
```
make test
```
## Exécution par défaut (équivalent) : 
```
./compress img/taric.png
./compress img/taric.png img/output.png
```
## Options :
 * p : Choix de la palette de couleurs
 * d : Choix de l'algorithme de diffusion d'erreur
 * s : Choix du coefficient de réduction de l'image (entier naturel) (réduction en largeur)
 * x : Ouverture de l'image dans la visionneuse d'image par défault d'ubuntu (Eye of Gnome)
#### Arguments de l'options "-p" :
  * 8 : Palette de 8 couleurs : saturation
  * 16 : Palette de 16 couleurs : CGA
  * 64 : Palette de 64 couleurs : 4-4-4
  * 216 : Palette de 216 couleurs : 6-6-6
  * 252 : Palette de 252 couleurs : 6-7-6
  * 2 : Palette de 2 couleurs : noir et blanc
  * 256 : Palette de 256 couleurs : niveaux de gris
#### Arguments de l'options "-d" :
  * 0 : Naïf
  * 1 : Floyd-Steinberg
  * 2 : Atkinson (comme Mister Bean)
  * 3 : Ordonnée
#### Exemple d'utilisation d'options :
```
./a.out img/taric.png -s 2 -p 252 -d 2 -x
```
## Convertir des .jpg en .png (writergba)
#### Le rendre exécutable :
```
sudo chmod +x ./writergba
```
#### Utilisation
```
./writergba img/taric.jpg img/taric.png
```
## Débogage avec Valgrind
#### Installation de valgrind
```
sudo apt install valgrind
```
#### Compilation (valgrind) : 
```
make debug
```
#### Exécution (valgrind) : 
```
valgrind ./compress_debug img/taric.png
```
