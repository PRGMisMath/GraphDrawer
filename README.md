# GraphDrawer
Petite application pour dessiner des graphes faite avec ImGUI et SFML.
L'idée est de créer une solution simple pour la création graphiquede graphe.

## Fonctionnement
Les graphes sont implémentés par une liste d'adjacence.
La liste principale contenant les points est implémentée par un vecteur dynamique couplé avec un système de défragmentation.
Les listes secondaires contenant les voisins sont implémentées par des listes monodirectionnelle.
Cela permet d'avoir une complexité en O(1) pour l'ajout et la suppression de sommets et pour l'ajout d'arêtes et une complexité en O(nb voisin) pour la suppression d'arête.
Un système de pointeur sur les sommets permet d'éviter d'avoir à parcourrir plusieurs fois la liste pour trouver les sommets et de ne pas perdre les éléments après une  défragmentation.
