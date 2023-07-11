#ifndef GRAPH_CORE_HPP
#define GRAPH_CORE_HPP

#include<iostream>

/*
* Gestion du graphe
* 
* Classe :
*	- Graphe
*	- Donnée
*/


/********* Constantes ********/

#define DEF_INIT_RESERVE_SIZE 20UL		//!< Taille par défaut pour l'initialistaion d'un graphe
#define AUTO_FRAGM_LIMIT .5f			//!< Taux de fragmentation seuil pour la défragmentation auto
#define MULT_COEFF_SIZE 2				//!< Coefficient multiplicateur de la taille lorsque le graphe est plein (ou presque)



/*********** Types ***********/

typedef unsigned long id_num;
class Graph;
//class Graph::NeighboorList;
class DataType;

class NotFound : public std::exception {};
class InvalidIndex : public std::exception {};
class AllocationError : public std::exception {};


class Graph {
public:

	/**
	* \brief Classe stockant un pointeur vers un élément dans le graphe.
	* Permet le manipulation des objets directement
	* sans avoir à appeler plusieurs fois des fonctions de recherche dans le graphe.
	*/
	typedef id_num* Node;

private:

	/**
	* \brief Classe codant une liste monochainé pour gérer les voisins de chaque noeud.
	* \details 
	* La classe contient un indicateur `m_nghb_id` de l'élément auquel il est relié
	* et un pointeur vers l'élément suivant de la liste. 
	* La fin de la liste est marqué par un pointeur NULL. 
	* \todo Pondération ?
	*/
	class NeighboorList {
	public:

		/**
		* \brief Initialisateur de NeighboorList
		* \details Crée la liste avec pour tête `id` et pour queue `next`.
		* \return La liste `id::next`
		*/
		static NeighboorList* allocNeighboor(Node id, NeighboorList* next, bool direct);

		/**
		* \brief Renvoie l'identifiant de l'élément en question (soit l'id du voisin).
		* \return L'identifiant du voisin.
		*/
		id_num nghbId();

		/**
		* \brief Renvoie le pointeur de l'élément en question (soit celui du voisin).
		* \return Le pointeur vers le voisin.
		*/
		Node nghbNode();

		/**
		* \brief Donne l'élément suivant de la liste.
		* \return Le prochain élément dans la liste.
		*/
		NeighboorList*& next();

		/**
		* \brief Vérifie si le lien est direct ou non pour un graphe orienté.
		* \return Vrai si le lien est direct.
		*/
		bool isDirect() const;

		/**
		* \brief Teste si la liste est finie ie si il n'y pas d'élémet après.
		* \return Vrai si la liste est finie.
		*/
		bool isFinished() const;

		/**
		* \brief Libère toute la liste (l'élément compris).
		* \details
		* Libère itérativement toute la liste.
		* Cette fonction détruit l'objet lui-même 
		* donc l'utiliser après sa destruction engendre des comportements imprévisibles.
		*/
		void freeAll();

	private:

		/**
		* \brief Identifiant de l'élément auquel il est relié.
		*/
		Graph::Node m_nghb_node;

		/**
		* \brief Elément suivant de la liste : vaut NULL si fin de liste.
		*/
		Graph::NeighboorList* m_next;

		/**
		* \brief Booléen indiquant dans le cas d'un graphe orienté si le lien est direct ou non
		*/
		bool m_direct;

	};


	/**
	* \brief Structure contenant les informations sur un sommet du graphe.
	*/
	struct Vertice {
		Node graph_id; //!< Identifiant dans le graphe
		id_num content_id; //!< Contenu en donnée d'un sommet
		Graph::NeighboorList* neighboor; //!< Liste d'adjacence
		struct {
			float x, y;
		} pos; //!< Position sur le plan (pas implémenté)
	};


protected:

	void ExpandReservE(int coef_mult = MULT_COEFF_SIZE);

	void DefragmenT();

	bool DeleteEdgE(Node from_id, Node to_id, bool direct);

public:

	/**
	* \brief Constructeur de Graph avec un buffer de taille fixée à `reserve_size`.
	* \param[in] oriented Indique si le graphe est orienté ou non
	* \param[in] reserve_size Taille réservée pour le buffer
	*/
	Graph(bool oriented = false, unsigned long reserve_size = DEF_INIT_RESERVE_SIZE);

	/**
	* \brief Destructeur de Graph
	*/
	~Graph();

	/**
	* \brief Ajoute un sommet au graphe.
	* \param[in] content_id Identifiant du contenu.
	* \return L'identifiant du nouveau sommet
	* \todo Remplacer content_id par content
	*/
	Node addVertice(id_num content_id);

	/**
	* \brief Enlève le sommet d'id correspondant ainsi que tout les arêtes allant vers lui.
	* \param[in] id Identifiant du sommet à supprimer
	*/
	void supprVertice(Node id);

	/**
	* \brief Ajoute une arête entre 2 sommets (si ils ne sont déjà pas reliés).
	* \details
	* Ajoute dans la liste de voisins de `from_id` une arête vers `to_id` 
	* ainsi que son correspondant dans la liste de voisins de `to_id`.
	* Ceci n'est fait que si les 2 sommets ne sont pas déjà connectés.
	* Dans le cas d'un graphe orienté pour une arête bidirectionnelle,
	* on trouvera les arêtes en double (pour pouvoir avoir des pondérations différentes).
	* \param[in] from_id Sommet de départ
	* \param[in] to_id Sommet d'arrivée
	* \return Vrai si l'arête n'était pas déjà présente.
	*/
	bool addEdge(Node from_id, Node to_id);

	/**
	* \brief Supprime une arête entre 2 sommets.
	* \param[in] from_id Sommet de départ
	* \param[in] to_id Sommet d'arrivée
	* \return Vrai si l'arête est déjà présente et si elle a été supprimée.
	*/
	bool supprEdge(Node from_id, Node to_id);

	/**
	* \brief Vérife si 2 sommets sont connectés par une arête.
	* \details 
	* Teste l'existence d'une arête reliant les 2 sommets.
	* Dans le cas d'un graphe non orienté, l'option `orientation` n'a aucun effet
	* car tous les arêtes sont directes.
	* Dans le cas d'un graphe orienté, l'option permet de prendre ou non en compte l'orientation.
	* \param[in] from_id Sommet de départ
	* \param[in] to_id Sommet d'arrivée
	* \param[in] orientation Si l'on souhaite prendre en compte l'orientation ou non.
	* \return Vrai si il y a une arête entre les 2 sommets.
	*/
	bool isLinked(Node from_id, Node to_id, bool orientation = true);

	/**
	* \brief Cherche un élément dans la liste par son contenu.
	* \return Le pointeur vers l'élément recherché.
	*/
	Node findVertice(id_num content_id);

	
	friend std::ostream& operator<<(std::ostream& os, const Graph& graph);

private:
	
	/**
	* \brief Structure contenant vers un `GraphVertices`.
	* \details
	* Cette structure est un artifice de programmation qui va permettre de stocker 
	* tous les sommets dans une liste C. 
	* L'idée est de mettre les sommets dans la liste et d'indiquer via un booléen
	* si la case est allouée ou non.
	* Ainsi lors de la destruction d'élément, on pourra éviter d'avoir à renuméroter tous les voisins
	* quand la liste est décaller d'un cran. 
	* On ajoutera un système de défragmentation et un indicateur basique de positions libres pour l'ajout.
	*/
	struct VerticeBlock {
		bool occupied;
		Vertice vertice;
	};

	/**
	* \brief Pointeur vers la zone mémoire réservée
	*/
	VerticeBlock* m_vertices;

	/**
	* \brief Taille de la zone mémoire réservée en nombre de `VerticeBlock`
	*/
	unsigned long m_reserve_size;

	/**
	* \brief Indicateur de la prochaine zone libre
	*/
	unsigned long m_free_pointeur;

	/**
	* \brief Nombre de sommets du graphe
	*/
	unsigned long m_nb_vertices;

	/**
	* \brief Booléen indiquant si le graphe est orienté
	*/
	bool m_oriented;

};


/**
* \brief Gère l'affichage du graphe dans l'invite de commande
*/
std::ostream& operator<<(std::ostream& os, const Graph& graph);


#endif
