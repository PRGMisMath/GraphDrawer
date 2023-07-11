#include "GraphCore.hpp"

#include<cstring>
#include<new>


Graph::NeighboorList* Graph::NeighboorList::allocNeighboor(Graph::Node id, NeighboorList* next, bool direct)
{
	Graph::NeighboorList* resPoint = (Graph::NeighboorList*)malloc(sizeof(Graph::NeighboorList));
	if (resPoint == NULL)
		throw std::bad_alloc();
	resPoint->m_next = next;
	resPoint->m_nghb_node = id;
	resPoint->m_direct = direct;

	return resPoint;
}

id_num Graph::NeighboorList::nghbId()
{
	return *this->m_nghb_node;
}

Graph::Node Graph::NeighboorList::nghbNode()
{
	return m_nghb_node;
}

Graph::NeighboorList*& Graph::NeighboorList::next()
{
	return this->m_next;
}

bool Graph::NeighboorList::isDirect() const
{
	return m_direct;
}

bool Graph::NeighboorList::isFinished() const
{
	return this->m_next == NULL;
}

void Graph::NeighboorList::freeAll()
{
	auto pointeur = this;
	auto next = pointeur;
	while (pointeur != NULL) {
		next = pointeur->m_next;
		free(pointeur);
		pointeur = next;
	}
}

void Graph::ExpandReservE(int coef_mult)
{
	// La défragmentation permet d'éviter à avoir à s'occuper de tous problèmes d'indice
	// et de pouvoir copier directement les données du nouveau à l'ancien pointeur.
	this->DefragmenT();

	m_reserve_size *= coef_mult;
	VerticeBlock* new_vertices = (VerticeBlock*)malloc(m_reserve_size * sizeof(VerticeBlock));
	if (new_vertices == NULL)
		throw std::bad_alloc();
	memset(new_vertices, 0, m_reserve_size * sizeof(VerticeBlock));
	memcpy(new_vertices, m_vertices, m_free_pointeur * sizeof(VerticeBlock));
	free(m_vertices);
	m_vertices = new_vertices;
}

void Graph::DefragmenT()
{
	unsigned long free_slot = 0, sort_slot = m_nb_vertices;
	while (free_slot < m_nb_vertices) {
		if (m_vertices[free_slot].occupied)
			++free_slot;
		else {
			// Recherche du prochain élément à déplacer
			while (!m_vertices[sort_slot].occupied)
				++sort_slot;
			m_vertices[free_slot] = m_vertices[sort_slot];
			m_vertices[free_slot].occupied = false;

			// Changement de l'indice 
			*(m_vertices[free_slot].vertice.graph_id) = free_slot;

			++sort_slot;
			++free_slot;
		}
	}
}

bool Graph::DeleteEdgE(Node from_id, Node to_id, bool direct)
{
	Graph::NeighboorList** neighboor = &(m_vertices[*from_id].vertice.neighboor);
	Graph::NeighboorList* next;
	while (*neighboor != NULL) {
		if ((*neighboor)->nghbNode() == to_id && ((*neighboor)->isDirect() == direct)) {
			next = (*neighboor)->next();
			// Suppression de l'élément
			free(*neighboor);
			// Reconcaténation avec le reste de la suite
			*neighboor = next;
			return true;
		}
		neighboor = &((*neighboor)->next());
	}
	return false;
}

Graph::Graph(bool oriented, unsigned long reserve_size) :
	m_reserve_size(reserve_size),
	m_nb_vertices(), m_vertices(),
	m_oriented(oriented)
{
	m_vertices = (VerticeBlock*)malloc(reserve_size * sizeof(VerticeBlock));
	if (m_vertices == NULL)
		throw std::bad_alloc();
	memset(m_vertices, 0, reserve_size * sizeof(VerticeBlock));
}

Graph::~Graph()
{
	for (int i = 0; i < m_free_pointeur; ++i)
		if (m_vertices[i].occupied) {
			m_vertices[i].vertice.neighboor->freeAll();
			free(m_vertices[i].vertice.graph_id);
		}
	free(m_vertices);
}

Graph::Node Graph::addVertice(id_num content_id)
{
	if (m_nb_vertices < AUTO_FRAGM_LIMIT * m_free_pointeur)
		this->DefragmenT();
	if (m_reserve_size == m_free_pointeur)
		this->ExpandReservE();
	Node verticeId = (Node)malloc(sizeof(id_num));
	if (verticeId == NULL)
		throw std::bad_alloc();
	*verticeId = m_free_pointeur;
	m_vertices[m_free_pointeur] = { true, { verticeId, content_id, NULL, {} } };
	++m_free_pointeur;
	++m_nb_vertices;

	return verticeId;
}

void Graph::supprVertice(Node id)
{
	// Cas limite
	if (m_reserve_size <= *id)
		throw InvalidIndex();
	if (m_vertices[*id].vertice.graph_id != id)
		throw InvalidIndex();

	// Libération de l'emplacement
	m_vertices[*id].occupied = false;
	--m_nb_vertices;
	
	// Supression des arêtes vers le sommet à supprimer
	auto pointeur = m_vertices[*id].vertice.neighboor;
	Graph::NeighboorList* next;
	while (pointeur != NULL) {
		next = pointeur->next();
		DeleteEdgE(pointeur->nghbNode(), id, !m_oriented || !pointeur->isDirect());
		free(pointeur);
		pointeur = next;
	}

	// Mise à jour de la fragmentation
	while (m_free_pointeur>0 && !m_vertices[m_free_pointeur-1].occupied)
		--m_free_pointeur;

	// Libération de id
	free(id);
}

bool Graph::addEdge(Node from_id, Node to_id)
{
	if (*from_id >= m_reserve_size || *to_id >= m_reserve_size)
		throw InvalidIndex();
	if (m_vertices[*from_id].vertice.graph_id != from_id || m_vertices[*to_id].vertice.graph_id != to_id)
		throw InvalidIndex();
	if (!this->isLinked(from_id, to_id)) {
		m_vertices[*from_id].vertice.neighboor = Graph::NeighboorList::allocNeighboor(to_id, m_vertices[*from_id].vertice.neighboor, true);
		m_vertices[*to_id].vertice.neighboor = Graph::NeighboorList::allocNeighboor(from_id, m_vertices[*to_id].vertice.neighboor, !m_oriented);
		return true;
	}
	return false;
}

bool Graph::supprEdge(Node from_id, Node to_id)
{
	if (*from_id >= m_reserve_size || *to_id >= m_reserve_size)
		throw InvalidIndex();
	if (m_vertices[*from_id].vertice.graph_id != from_id || m_vertices[*to_id].vertice.graph_id != to_id)
		throw InvalidIndex();
	return DeleteEdgE(from_id, to_id, true) && DeleteEdgE(to_id, from_id, !m_oriented);
}

bool Graph::isLinked(Node from_id, Node to_id, bool orientation)
{
	if (*from_id >= m_reserve_size || *to_id >= m_reserve_size)
		throw InvalidIndex();
	if (m_vertices[*from_id].vertice.graph_id != from_id || m_vertices[*to_id].vertice.graph_id != to_id)
		throw InvalidIndex();
	auto pointeur = m_vertices[*from_id].vertice.neighboor;
	while (pointeur != NULL) {
		if (pointeur->nghbNode() == to_id && (!orientation || pointeur->isDirect()))
			return true;
		pointeur = pointeur->next();
	}
	return false;
}

Graph::Node Graph::findVertice(id_num content_id)
{
	for (int i = 0; i < m_free_pointeur; ++i)
		if (m_vertices[i].occupied && m_vertices[i].vertice.content_id == content_id)
			return m_vertices[i].vertice.graph_id;
	throw NotFound();
}

std::ostream& operator<<(std::ostream& os, const Graph& graph)
{
	for (int i = 0; i < graph.m_free_pointeur; ++i) {
		if (!graph.m_vertices[i].occupied)
			continue;
		os << "\nElement : " << graph.m_vertices[i].vertice.content_id
			<< "\n\tVoisins : [";
		auto pointeur = graph.m_vertices[i].vertice.neighboor;
		while (pointeur != NULL)
		{
			if (pointeur->isDirect())
				os << graph.m_vertices[pointeur->nghbId()].vertice.content_id << ';';
			pointeur = pointeur->next();
		}
		os << "NULL]\n";
	}
	return os;
}
