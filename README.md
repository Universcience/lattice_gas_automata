# `lattice_gas_automata`

Automates cellulaires modélisant le comportement des particules d'un gaz en 2D, développés dans le cadre de l'exposé **Informatique et autres sciences** du Palais de la Découverte.

## Description

Les deux programmes simulent le comportement d'un gaz en 2D enfermé dans une boîte rectangulaire par le biais d'[automates cellulaires dédiés(en)](https://en.wikipedia.org/wiki/Lattice_gas_automaton). Ces deux modèles, très simples, permettent de retrouver certaines notions clés du comportement des gaz tout en illustrant les limites de ce type de simulation, notamment par les artefacts visibles durant les premières secondes.

Le [modèle HPP(en)](https://en.wikipedia.org/wiki/Lattice_gas_automaton#Early_attempts_with_a_square_lattice), introduit dans les années 70 par Hardis, de Pazzi et Pomeau, utilise une grille rectangulaire pour le placement des particules, avec quatre directions possibles pour leur déplacement.

Le [modèle FHP(en)](https://en.wikipedia.org/wiki/Lattice_gas_automaton#Hexagonal_grids) de Frisch, Hasslacher et Pomeau, datant des années 80, utilise une grille triangulaire, avec un voisinage hexagonal donnant six directions possibles pour le mouvement des particules, améliorant la qualité de la simulation et réduisant considérablement les artefacts visuels.

# Déploiement

Le programme fourni est conçu pour un système GNU/Linux ([Debian(fr)](https://www.debian.org/index.fr.html)), mais peut être aisément porté sur d'autres systèmes.

Le fichier `Makefile` fourni permet de compiler les programmes C (cibles `hpp_model` et `fhp_model`).

## Paquetages nécessaires (Debian Stretch)
- `gcc`
- `make`
- `libsdl2-dev`

## Usage
```
git clone https://github.com/universcience/lattice_gas_automata
cd lattice_gas_automata
make
./hpp_model
./fhp_model
```

# Licence

Ceci est un [logiciel libre(fr)](https://www.gnu.org/philosophy/free-sw.fr.html), distribué sous les termes de la licence GNU GPL Affero. Les termes exacts de cette licence sont disponibles dans le fichier `LICENSE`.

En résumé, vous êtes libres de télécharger ce logiciel, de l'utiliser et de le modifier sans restrictions, ainsi que de le redistribuer sous les mêmes termes, avec ou sans modifications. 
