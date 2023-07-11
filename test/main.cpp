#include "GraphCore.hpp"
#include<iostream>


/*

Structure du projet :
 - Syst�me de graphe
   - Stockage en tant que donn�e (position exacte VS graphe pure)	XXXXXXXXXX (sauf pond�ration)
   - Sauvegarde et chargement en m�moire							__________
   - Dessin															__________
 - Syst�me d'interface
   - Gestion du menu												__________
   - Ajout de sommet et d'ar�te et de label							__________

*/






int main() {
	Graph graph{ true };
	Graph::Node nodes[20];

	for (int i = 0; i < 20; ++i)
		nodes[i]=graph.addVertice(i);

	std::cout << std::flush << "Ajout des points : " << graph;

	for (int i=0; i<20; ++i)
		for (int j=0; j<20; ++j)
			if (i != j && ((i * j + 3 * i * i) % 7) == 1) {
				graph.addEdge(nodes[i], nodes[j]);
			}

	std::cout << "\nAjout des ar�tes : " << graph;


	for (int i = 0; i < 20; ++i)
		for (int j = 0; j < 20; ++j)
			if (i != j && ((i * j * j + 2 * i * i) % 9) == 3) {
				graph.supprEdge(nodes[i], nodes[j]);
			}

	std::cout << "\nSuppression des ar�tes : " << graph;

	for (int i = 0; i < 20; ++i)
		if (((6 * i * i * i - 2 * i + 1) % 3) == 2) {
			graph.supprVertice(nodes[i]);
		}

	std::cout << "\nSuppression des points : " << graph;

	graph.addEdge(graph.addVertice(666), nodes[0]);

	std::cout << "\nTest de la d�fragmentation et de la r�allocation : " << graph;

	return 0;
}