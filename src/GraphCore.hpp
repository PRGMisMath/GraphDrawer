#ifndef GRAPH_CORE_HPP
#define GRAPH_CORE_HPP

#include<iostream>

/*
* Gestion du graphe
* 
* Classe :
*	- Graphe
*	- Donn�e
*/


/********* Constantes ********/

#define DEF_INIT_RESERVE_SIZE 20UL		//!< Taille par d�faut pour l'initialistaion d'un graphe
#define AUTO_FRAGM_LIMIT .5f			//!< Taux de fragmentation seuil pour la d�fragmentation auto
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
	* \brief Classe stockant un pointeur vers un �l�ment dans le graphe.
	* Permet le manipulation des objets directement
	* sans avoir � appeler plusieurs fois des fonctions de recherche dans le graphe.
	*/
	typedef id_num* Node;

private:

	/**
	* \brief Classe codant une liste monochain� pour g�rer les voisins de chaque noeud.
	* \details 
	* La classe contient un indicateur `m_nghb_id` de l'�l�ment auquel il est reli�
	* et un pointeur vers l'�l�ment suivant de la liste. 
	* La fin de la liste est marqu� par un pointeur NULL. 
	* \todo Pond�ration ?
	*/
	class NeighboorList {
	public:

		/**
		* \brief Initialisateur de NeighboorList
		* \details Cr�e la liste avec pour t�te `id` et pour queue `next`.
		* \return La liste `id::next`
		*/
		static NeighboorList* allocNeighboor(Node id, NeighboorList* next, bool direct);

		/**
		* \brief Renvoie l'identifiant de l'�l�ment en question (soit l'id du voisin).
		* \return L'identifiant du voisin.
		*/
		id_num nghbId();

		/**
		* \brief Renvoie le pointeur de l'�l�ment en question (soit celui du voisin).
		* \return Le pointeur vers le voisin.
		*/
		Node nghbNode();

		/**
		* \brief Donne l'�l�ment suivant de la liste.
		* \return Le prochain �l�ment dans la liste.
		*/
		NeighboorList*& next();

		/**
		* \brief V�rifie si le lien est direct ou non pour un graphe orient�.
		* \return Vrai si le lien est direct.
		*/
		bool isDirect() const;

		/**
		* \brief Teste si la liste est finie ie si il n'y pas d'�l�met apr�s.
		* \return Vrai si la liste est finie.
		*/
		bool isFinished() const;

		/**
		* \brief Lib�re toute la liste (l'�l�ment compris).
		* \details
		* Lib�re it�rativement toute la liste.
		* Cette fonction d�truit l'objet lui-m�me 
		* donc l'utiliser apr�s sa destruction engendre des comportements impr�visibles.
		*/
		void freeAll();

	private:

		/**
		* \brief Identifiant de l'�l�ment auquel il est reli�.
		*/
		Graph::Node m_nghb_node;

		/**
		* \brief El�ment suivant de la liste : vaut NULL si fin de liste.
		*/
		Graph::NeighboorList* m_next;

		/**
		* \brief Bool�en indiquant dans le cas d'un graphe orient� si le lien est direct ou non
		*/
		bool m_direct;

	};


	/**
	* \brief Structure contenant les informations sur un sommet du graphe.
	*/
	struct Vertice {
		Node graph_id; //!< Identifiant dans le graphe
		id_num content_id; //!< Contenu en donn�e d'un sommet
		Graph::NeighboorList* neighboor; //!< Liste d'adjacence
		struct {
			float x, y;
		} pos; //!< Position sur le plan (pas impl�ment�)
	};


protected:

	void ExpandReservE(int coef_mult = MULT_COEFF_SIZE);

	void DefragmenT();

	bool DeleteEdgE(Node from_id, Node to_id, bool direct);

public:

	/**
	* \brief Constructeur de Graph avec un buffer de taille fix�e � `reserve_size`.
	* \param[in] oriented Indique si le graphe est orient� ou non
	* \param[in] reserve_size Taille r�serv�e pour le buffer
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
	* \brief Enl�ve le sommet d'id correspondant ainsi que tout les ar�tes allant vers lui.
	* \param[in] id Identifiant du sommet � supprimer
	*/
	void supprVertice(Node id);

	/**
	* \brief Ajoute une ar�te entre 2 sommets (si ils ne sont d�j� pas reli�s).
	* \details
	* Ajoute dans la liste de voisins de `from_id` une ar�te vers `to_id` 
	* ainsi que son correspondant dans la liste de voisins de `to_id`.
	* Ceci n'est fait que si les 2 sommets ne sont pas d�j� connect�s.
	* Dans le cas d'un graphe orient� pour une ar�te bidirectionnelle,
	* on trouvera les ar�tes en double (pour pouvoir avoir des pond�rations diff�rentes).
	* \param[in] from_id Sommet de d�part
	* \param[in] to_id Sommet d'arriv�e
	* \return Vrai si l'ar�te n'�tait pas d�j� pr�sente.
	*/
	bool addEdge(Node from_id, Node to_id);

	/**
	* \brief Supprime une ar�te entre 2 sommets.
	* \param[in] from_id Sommet de d�part
	* \param[in] to_id Sommet d'arriv�e
	* \return Vrai si l'ar�te est d�j� pr�sente et si elle a �t� supprim�e.
	*/
	bool supprEdge(Node from_id, Node to_id);

	/**
	* \brief V�rife si 2 sommets sont connect�s par une ar�te.
	* \details 
	* Teste l'existence d'une ar�te reliant les 2 sommets.
	* Dans le cas d'un graphe non orient�, l'option `orientation` n'a aucun effet
	* car tous les ar�tes sont directes.
	* Dans le cas d'un graphe orient�, l'option permet de prendre ou non en compte l'orientation.
	* \param[in] from_id Sommet de d�part
	* \param[in] to_id Sommet d'arriv�e
	* \param[in] orientation Si l'on souhaite prendre en compte l'orientation ou non.
	* \return Vrai si il y a une ar�te entre les 2 sommets.
	*/
	bool isLinked(Node from_id, Node to_id, bool orientation = true);

	/**
	* \brief Cherche un �l�ment dans la liste par son contenu.
	* \return Le pointeur vers l'�l�ment recherch�.
	*/
	Node findVertice(id_num content_id);

	
	friend std::ostream& operator<<(std::ostream& os, const Graph& graph);

private:
	
	/**
	* \brief Structure contenant vers un `GraphVertices`.
	* \details
	* Cette structure est un artifice de programmation qui va permettre de stocker 
	* tous les sommets dans une liste C. 
	* L'id�e est de mettre les sommets dans la liste et d'indiquer via un bool�en
	* si la case est allou�e ou non.
	* Ainsi lors de la destruction d'�l�ment, on pourra �viter d'avoir � renum�roter tous les voisins
	* quand la liste est d�caller d'un cran. 
	* On ajoutera un syst�me de d�fragmentation et un indicateur basique de positions libres pour l'ajout.
	*/
	struct VerticeBlock {
		bool occupied;
		Vertice vertice;
	};

	/**
	* \brief Pointeur vers la zone m�moire r�serv�e
	*/
	VerticeBlock* m_vertices;

	/**
	* \brief Taille de la zone m�moire r�serv�e en nombre de `VerticeBlock`
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
	* \brief Bool�en indiquant si le graphe est orient�
	*/
	bool m_oriented;

};


/**
* \brief G�re l'affichage du graphe dans l'invite de commande
*/
std::ostream& operator<<(std::ostream& os, const Graph& graph);


#endif
