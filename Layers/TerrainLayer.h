#ifndef TERRAINLAYER_H
#define TERRAINLAYER_H

#include "Quadtree.h"
#include "Layer.h"
#include "OpenGL/Shaders/GLShader.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <QThread>


/**
 * @brief This class is used to read and display terrain data from a fort.14 file
 *
 * This class is responsible for all data associated with an ADCIRC fort.14 file. It is
 * capable of reading the file, storing the data, and quickly accessing and drawing the
 * data as needed.
 *
 */
class TerrainLayer : public Layer
{
		Q_OBJECT
	public:

		// Constructor/Destructor
		TerrainLayer();
		~TerrainLayer();

		// Virtual methods to override
		virtual void	Draw();
		virtual void	LoadDataToGPU();

		// Getter Methods
		std::string		GetFort14Location();
		Node*			GetNode(unsigned int nodeNumber);
		Node*			GetNode(float x, float y);
		std::vector<Node*>	GetNodesFromCircle(float x, float y, float radius);
		Element*		GetElement(unsigned int elementNumber);
		Element*		GetElement(float x, float y);
		unsigned int		GetNumNodes();
		unsigned int		GetNumElements();
		float			GetMinX();
		float			GetMaxX();
		float			GetMinY();
		float			GetMaxY();
		float			GetMinZ();
		float			GetMaxZ();
		float			GetUnprojectedX(float x);
		float			GetUnprojectedY(float y);
		SolidShaderProperties		GetSolidOutline();
		SolidShaderProperties		GetSolidFill();
		GradientShaderProperties	GetGradientOutline();
		GradientShaderProperties	GetGradientFill();

		// Setter Methods
		virtual void	SetCamera(GLCamera *newCamera);
		void		SetFort14Location(std::string newLocation);
		void		SetSolidOutline(SolidShaderProperties newProperties);
		void		SetSolidFill(SolidShaderProperties newProperties);
		void		SetGradientOutline(GradientShaderProperties newProperties);
		void		SetGradientFill(GradientShaderProperties newProperties);



	protected:

		// Terrain Specific Variables
		std::string		fort14Location; /**< The absolute path of the fort.14 file */
		std::vector<Node>	nodes;		/**< List of all Nodes in the Layer */
		std::vector<Element>	elements;	/**< List of all Elements in the Layer */
		std::string		infoLine;	/**< The info line in the fort.14 file */
		unsigned int		numNodes;	/**< The number of Nodes in the Layer as specified in fort.14 */
		unsigned int		numElements;	/**< The number of Elements in the Layer as specified in fort.14 */
		float			minX;		/**< The minimum x-value */
		float			midX;		/**< The calculated center of the x-values */
		float			maxX;		/**< The maximum x-value */
		float			minY;		/**< The minimum y-value */
		float			midY;		/**< the calculated center of the y-values */
		float			maxY;		/**< The maximum y-value */
		float			minZ;		/**< The minimum z-value */
		float			maxZ;		/**< The maximum z-value */
		float			max;		/**< The maximum of the width/height of the domain */

		/* All shaders needed to draw a terrain layer */
		GLShader*	outlineShader;		/**< Pointer to the GLShader object that should be used to draw the outline */
		GLShader*	fillShader;		/**< Pointer to the GLShader object that should be used to draw the fill */

		// Terrain Specific OpenGL Variables
		GLuint		VAOId;			/**< The vertex array object ID in the OpenGL context */
		GLuint		VBOId;			/**< The vertex buffer object ID in the OpenGL context */
		GLuint		IBOId;			/**< The index buffer object ID in the OpenGL context */

		// Flags
		bool	flipZValue;		/**< Flag that determines if the z-value is multiplied by -1 before being loaded to the GPU */
		bool	fileLoaded;		/**< Flag that shows if data has been successfully read from the fort.14 file */
		bool	glLoaded;		/**< Flag that shows if data has been successfully sent to the GPU */

		// Picking Variables
		Quadtree*	quadtree;	/**< The quadtree used for Node picking */

	private:

		SolidShader*	solidOutline;		/**< Shader used to draw a solid outline */
		SolidShader*	solidFill;		/**< Shader used to draw a solid fill */
		GradientShader*	gradientOutline;	/**< Shader used to draw a gradient outline */
		GradientShader*	gradientFill;		/**< Shader used to draw a gradient fill */

	public slots:

		// Slots used for work that needs to be performed on a worker thread
		void	readFort14();		/**< Reads the fort.14 file */

	signals:

		// Signals used during threaded reading of fort.14
		void	fort14Valid();
		void	foundNumNodes(int);
		void	foundNumElements(int);
		void	finishedLoadingToGPU();
		void	error(QString err);
};

#endif // TERRAINLAYER_H
