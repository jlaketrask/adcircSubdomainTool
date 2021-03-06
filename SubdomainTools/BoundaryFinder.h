#ifndef BOUNDARYFINDER_H
#define BOUNDARYFINDER_H

#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include "adcData.h"
#include "Layers/Actions/ElementState.h"

struct Edge
{
		unsigned int n1;
		unsigned int n2;
		Edge() : n1(0), n2(0) {}
		Edge(unsigned int first, unsigned int second)
		{
			if (first < second)
			{
				n1 = first;
				n2 = second;
			} else {
				n1 = second;
				n2 = first;
			}
		}
		bool operator== (const Edge &e1) const{
			return (n1 == e1.n1 && n2 == e1.n2);
		}
		bool operator< (const Edge &e1) const{
			if (n1 == e1.n1)
				return n2 < e1.n2;
			return n1 < e1.n1;
		}
};


struct Boundaries
{
		std::set<unsigned int>	innerBoundaryNodes;
		std::set<unsigned int>	outerBoundaryNodes;
};

class BoundaryFinder
{
	public:
		/* Constructor */
		BoundaryFinder();
		~BoundaryFinder();

		/* The Callable Search Function */
		std::vector<unsigned int> FindBoundaries(ElementState* elementSelection);
		std::vector<unsigned int> FindInnerBoundaries(ElementState* elementSelection);
		Boundaries	FindAllBoundaries(std::vector<Element> *elements);

	private:

		std::map<Edge, int>					edgesMap;
		std::map<unsigned int, std::vector<unsigned int> >	nodeAdjacency;
		std::vector<unsigned int>				edgesList;

		void	FindEdges(std::vector<Element*>* elements);
		void	CreateEdgesList();

};

#endif // BOUNDARYFINDER_H
