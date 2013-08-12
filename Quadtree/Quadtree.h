#ifndef QUADTREE_H
#define QUADTREE_H

#include "OpenGL/GLCamera.h"
#include "OpenGL/Shaders/SolidShader.h"
#include "adcData.h"
#include "QuadtreeData.h"
#include <vector>
#include <math.h>

#include "Quadtree/SearchTools/PolygonSearch.h"

/**
 * @brief This class provides a data structure that can be used to store a large number
 * of Node objects and provide very quick access to the node closest to a specific point.
 *
 * This class allows the user to store a large number of Node objects in a quadtree data
 * structure. This provides very fast lookup when the user needs to find the Node that is
 * closest to a specific point in space. Nodes are stored in bins of a specific size
 * (called leaves) based on their physical location in space. Once a leaf reaches its
 * maximum size, if another Node attempts to enter that leaf, the leaf is turned into
 * a branch. A branch has four spaces that can hold data, each of which can either be
 * a leaf or another branch. The result is a tree structure that can be quickly traversed
 * using x-y coordinates. Once a leaf has been reached in the tree traversal, a linear
 * search is done on all of the Nodes in that leaf to find the one closest to the
 * provided x-y coordinates.
 *
 * In order for this data structure to be effective, the dataset must be very large
 * and the bin size chosen such that the time spent doing a linear search at the
 * leaf level is negligible.
 *
 * The class currently has its own copy of all nodal data, which can hog a lot of memory
 * and cpu time performing a copy of a very large dataset, so a future optimization could
 * be to pass only a pointer to the existing data. This could be potentially dangerous
 * if the nodal data is modified after the Quadtree is created and before a call to
 * Quadtree::FindNode() is performed.
 *
 */
class Quadtree
{
	public:

		// Constructor/Destructor
		Quadtree(std::vector<Node> nodes, int size, float minX, float maxX, float minY, float maxY);
		Quadtree(std::vector<Node> nodes, std::vector<Element> elements, int size, float minX, float maxX, float minY, float maxY);
		~Quadtree();

		/* Drawing Functions */
		void	DrawOutlines();
		void	SetCamera(GLCamera* newCam);

		// Public Functions
		Node*			FindNode(float x, float y);
		std::vector<Node*>	FindNodesInCircle(float x, float y, float radius);
		std::vector<Element*>	FindElementsInCircle(float x, float y, float radius);
		std::vector<Element*>	FindElementsInRectangle(float l, float r, float b, float t);
		std::vector<Element*>	FindElementsInPolygon(std::vector<Point> polyLine);
		std::vector<std::vector<Element*> *> GetElementsThroughDepth(int depth);
		std::vector<std::vector<Element*> *> GetElementsThroughDepth(int depth, float l, float r, float b, float t);

	private:

		// Data Variables
		int			binSize;	/**< The maximum number of Nodes allowed in a leaf */
		std::vector<Node>	nodeList;	/**< The list of all Nodes in the domain */
		std::vector<Element>	elementList;	/**< The list of all Elements in the domain */
		std::vector<branch*>	branchList;	/**< The list of all branches in the Quadtree */
		std::vector<leaf*>	leafList;	/**< The list of all leaves in the Quadtree */
		branch*			root;		/**< A pointer to the top of the Quadtree */
		bool			hasElements;	/**< Flag that shows if the Quadtree contains Element data */

		/* Search Tools */
		PolygonSearch	polySearch;

		/* Drawing Variables */
		bool	glLoaded;
		int	pointCount;
		GLuint	VAOId;
		GLuint	VBOId;
		GLuint	IBOId;
		SolidShader*	outlineShader;
		GLCamera*	camera;

		/* Drawing Functions */
		void			InitializeGL();
		std::vector<Point>	BuildOutlinesList();
		std::vector<GLuint>	BuildOutlinesIndices();
		void			AddOutlinePoints(branch *currBranch, std::vector<Point> *pointsList);
		void			AddOutlinePoints(leaf *currLeaf, std::vector<Point> *pointsList);
		void			LoadOutlinesToGPU(std::vector<Point> pointsList, std::vector<GLuint> indicesList);

		////// Recursive searching functions
		Node*	FindNode(float x, float y, branch *currBranch);
		void	FindLeavesInCircle(float x, float y, float radius, branch *currBranch, std::vector<leaf*>* full, std::vector<leaf*>* partial);
		void	FindLeavesInRectangle(float l, float r, float b, float t, branch *currBranch, std::vector<leaf*>* full, std::vector<leaf*>* partial);
		void	AddAllLeaves(branch *currBranch, std::vector<leaf*>* full);
		void	AddFullNodes(std::vector<leaf*>* full, std::vector<Node*>* nodes);
		void	AddFullElements(std::vector<leaf*>* full, std::vector<Element*>* elements);

		// Finding Nodes within a circle
		void	AddPartialNodes(float x, float y, float radius, std::vector<leaf*>* partial, std::vector<Node*>* nodes);

		// Finding Elements within a circle
		void	AddPartialElements(float x, float y, float radius, std::vector<leaf*>* partial, std::vector<Element*>* elements);

		// Finding Elements within a rectangle
		void	AddPartialElements(float l, float r, float b, float t, std::vector<leaf*>* partial, std::vector<Element*>* elements);

		// Retrieving Elements recursively
		void	RetrieveElements(branch* currBranch, int depth, std::vector<std::vector<Element *> *> *list);
		void	RetrieveElements(branch* currBranch, int depth, std::vector<std::vector<Element *> *> *list, float l, float r, float b, float t);

		////// Building functions
		leaf*	newLeaf(float l, float r, float b, float t);
		branch*	newBranch(float l, float r, float b, float t);
		branch*	leafToBranch(leaf *currLeaf);
		branch*	addNode(Node *currNode, leaf *currLeaf);
		void	addNode(Node *currNode, branch *currBranch);
		void	addElement(Element *currElement, branch *currBranch);
		bool	nodeIsInside(Node *currNode, leaf *currLeaf);
		bool	nodeIsInside(Node *currNode, branch *currBranch);

		////// Helper functions
		float	distance(Node *currNode, float x, float y);
		bool	pointIsInside(leaf *currLeaf, float x, float y);
		bool	pointIsInside(branch *currBranch, float x, float y);
		bool	pointIsInside(float l, float r, float b, float t, float x, float y);
		bool	pointIsInsideCircle(float x, float y, float circleX, float circleY, float radius);
		bool	rectangleIntersection(leaf *currLeaf, float l, float r, float b, float t);
		bool	rectangleIntersection(branch *currBranch, float l, float r, float b, float t);
		bool	branchIsInside(branch *currBranch, float l, float r, float b, float t);
		bool	leafIsInside(leaf *currLeaf, float l, float r, float b, float t);
		bool	rectangleIsInside(float l, float r, float b, float t, branch *currBranch);
		bool	rectangleIsInside(float l, float r, float b, float t, leaf *currLeaf);
};

#endif // QUADTREE_H