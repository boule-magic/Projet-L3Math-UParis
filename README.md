# Projet-L3Math-UParis

Il s'agit d'un petit programme de compression d'image qui réduit une image PNG en une image PNG à palette de couleur choisi. Pour un meilleur rendu il se sert des algorithmes de diffusion d'erreur de Floyd-Steinberg, d'Atkinson et ordonné. Il peut également modifier la taille de l'image par ré-échantillonnage bilinéaire.

| image | result | static/dynamic | colors | dithering |
| --- | --- | --- | --- | --- |
| source |  ![aardvark.png](samples/aardvark.png) | | | |
| blanck & white |  ![sample_static_2.png](samples/sample_static_2.png) | static | 2 | no |
| blanck & white |  ![sample_static_2_atkinson.png](samples/sample_static_2_atkinson.png) | static | 2 | atkinson |
| saturated |  ![sample_static_8.png](samples/sample_static_8.png) | static | 8 | no |
| saturated |  ![sample_static_8_atkinson.png](samples/sample_static_8_atkinson.png) | static | 8 | atkinson |
| 252 colors |  ![sample_static_252.png](samples/sample_static_252.png) | static | 252 | no |
| 252 colors  |  ![sample_static_252_atkinson.png](samples/sample_static_252_atkinson.png) | static | 256 | atkinson
| 2 colors  |  ![sample_dynamic_2.png](samples/sample_dynamic_2.png) | dynamic | 2 | no |
| 256 colors  |  ![sample_dynamic_256.png](samples/sample_dynamic_256.png) | dynamic | 256 | no |

## Compilation
#### Setup : 
```
sudo apt install gcc make
```
#### Compiler : 
```
make
```
## Tester
```
make samples
```
```
make dithering
```
```
make scaling
```
```
make dynamic
```
## Help
```
./compresspng
```
## Exécution par défaut (équivalent) : 
```
./compresspng img/taric.png
./compresspng img/taric.png img/output.png
```
## Options :
 * p \[arg\] : Choix de la taille de la palette de couleurs statique
 * P \[arg\] : Choix de la taille de la palette de couleur dynamique (median cut par defaut)
 * f         : Permet de changer le mode de génération de palette dynamique en mode "fast" (histogramme)
 * d \[arg\] : Choix de l'algorithme de diffusion d'erreur
 * h \[arg\] : Choix de la hauteur de l'image de sortie (en pixel)
 * w \[arg\] : Choix de la largeur de l'image de sortie (en pixel)
 * x         : Ouverture de l'image dans la visionneuse d'image par défault d'ubuntu (Eye of Gnome)
#### Arguments de l'options "-p" :
  * 2 : Palette de 2 couleurs : noir et blanc
  * 8 : Palette de 8 couleurs : saturation
  * 16 : Palette de 16 couleurs : CGA
  * 64 : Palette de 64 couleurs : 4-4-4
  * 216 : Palette de 216 couleurs : 6-6-6
  * 252 : Palette de 252 couleurs : 6-7-6
  * 256 : Palette de 256 couleurs : niveaux de gris
#### Arguments de l'options "-d" :
  * 0 : Naïf
  * 1 : Floyd-Steinberg
  * 2 : Atkinson (comme Mister Bean)
  * 3 : Ordonnée 
#### Exemple d'utilisation d'options :
```
./compresspng img/taric.png -p 252 -d 2 -h 800 -x
```
## Writergba : Convertir des images au format lisible par compresspng (.png en rgba)
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
valgrind ./compresspng_debug img/taric.png
```
