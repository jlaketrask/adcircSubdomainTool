#ifndef DOMAIN_H
#define DOMAIN_H

#include <QObject>
#include <QThread>
#include <QProgressBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <string>
#include <vector>

#include "Layers/Layer.h"
#include "Layers/TerrainLayer.h"
#include "Layers/SelectionLayers/CreationSelectionLayer.h"

#include "OpenGL/GLCamera.h"
#include "OpenGL/Shaders/GLShader.h"
#include "OpenGL/Shaders/SolidShader.h"
#include "OpenGL/Shaders/GradientShader.h"

#include "Projects/ProjectFile.h"


/**
 * @brief This class is used to represent an ADCIRC domain (either a full or subdomain).
 *
 * This class is used to represent an ADCIRC domain (either a full or subdomain). An instance
 * of this class contains everything needed to load and interact with all of the data associated
 * with a single ADCIRC run.
 *
 * It is a very high level class, acting as a sort of umbrella used to oversee all functionality
 * and organization of the large amount of data it contains. As such, it provides an excellent
 * bridge between the GUI and the underlying data.
 *
 * <H2>Independence</H2>
 *
 * One of the goals of the Subdomain Modeling Tool is to give the user the ability to easily
 * manipulate many subdomains independently of one another in a single GUI. Therefore,
 * the following design decisions were made:
 * - Each Domain has its own GLCamera object, which allows the user to toggle between domains
 *   without changing the view of the individual domains
 * - Each Domain has its own SelectionLayer, which not only allows the user to maintain selections
 *   independently, but also provides an Undo/Redo stack for each Domain. This means that a user
 *   can make changes in one domain and switch to a different domain and undo changes that had
 *   previously occurred in that domain
 *
 * <H2>Qt Signal/Slot Functionality</H2>
 *
 * This class has been developed with the idea that it be used in the context of a Qt signal/slot
 * mechanism. However, all basic functionality has been written using basic functions so that
 * upon removal from the Qt environment, the class will still perform as expected.
 *
 * All of the signals and slots used by this class are only used to communicate state to GUI. Any
 * function that modifies the state of the class is implemented as a normal function.
 *
 */
class Domain : public QObject
{
		Q_OBJECT
	public:
		Domain();
		~Domain();

		// Drawing and Selection Interaction Functions
		void	Draw();
		void	MouseClick(QMouseEvent *event);
		void	MouseMove(QMouseEvent *event);
		void	MouseRelease(QMouseEvent *event);
		void	MouseWheel(QWheelEvent *event);
		void	KeyPress(QKeyEvent *event);
		void	SetWindowSize(float w, float h);
		void	UseTool(ToolType tool, SelectionType selection);
		void	Undo();
		void	Redo();

		// Modification functions used to set the state of the Domain based on GUI interaction
		void	SetProgressBar(QProgressBar* newBar);
		void	SetDomainPath(QString newPath);
		void	SetFort14Location(QString newLoc);
		void	SetFort15Location(QString newLoc);
		void	SetFort63Location(QString newLoc);
		void	SetFort64Location(QString newLoc);
		void	SetBNListLocation(QString newLoc);
		void	SetPy140Location(QString newLoc);

		// Query functions used to access data used to populate the GUI
		QString		GetDomainPath();
		QString		GetFort14Location();
		QString		GetFort15Location();
		QString		GetFort63Location();
		QString		GetFort64Location();
		QString		GetBNListLocation();
		QString		GetPy140Location();
		std::vector<Element> *GetAllElements();
		ElementState*	GetCurrentSelectedElements();
		float		GetTerrainMinElevation();
		float		GetTerrainMaxElevation();
		ShaderType	GetTerrainOutlineShaderType();
		ShaderType	GetTerrainFillShaderType();
		QColor		GetTerrainSolidOutline();
		QColor		GetTerrainSolidFill();
		QGradientStops	GetTerrainGradientOutline();
		QGradientStops	GetTerrainGradientFill();
		unsigned int	GetNumNodesDomain();
		unsigned int	GetNumElementsDomain();
		unsigned int	GetNumNodesSelected();
		unsigned int	GetNumElementsSelected();
		GLCamera*	GetCamera();

		/* Display methods used to change visibility of layers, etc. */
		void	ToggleTerrainQuadtree();


	private:

		ProjectFile*	projectFile;

		// Camera
		GLCamera*	camera;		/**< The camera used for all drawing operations (except the selection layer)*/

		// Layers
		TerrainLayer*		terrainLayer;	/**< The terrain layer */
		CreationSelectionLayer*	selectionLayer;	/**< The selection layer */

		// Loading Operations
		QThread*	layerThread;	/**< The thread on which file reading operations will execute */
		QProgressBar*	progressBar;	/**< The progress bar that will show file reading progress */
		Layer*		loadingLayer;	/**< Sort of a queue for the next layer that will send data to the GPU */

		void	LoadFort14File();

		/* Layer creation functions */
		void	CreateTerrainLayer();

		/* Domain Characteristics */
		QString		domainPath;
		QString		fort14Location;
		QString		fort15Location;
		QString		fort63Location;
		QString		fort64Location;
		QString		bnListLocation;
		QString		py140Location;

		/* Mouse Clicking and Moving Stuff */
		ActionType	currentMode;	/**< The current mode used to determine where actions are sent */
		int		oldx;	/**< The old x-coordinate of the mouse */
		int		oldy;	/**< The old y-coordinate of the mouse */
		int		newx;	/**< The new x-coordinate of the mouse */
		int		newy;	/**< The new y-coordinate of the mouse */
		int		dx;	/**< The change in x-coordinates between mouse events */
		int		dy;	/**< The change in y-coordinates between mouse events */
		Qt::MouseButton	pushedButton;	/**< The mouse button being pushed */
		bool		clicking;	/**< Flag that shows if a mouse button is being held down */
		bool		mouseMoved;	/**< Flag that shows if the mouse has moved since the last event */
		void		Zoom(float zoomAmount);
		void		Pan(float dx, float dy);
		void		CalculateMouseCoordinates();


	public slots:

		void	SetTerrainSolidOutline(QColor newColor);
		void	SetTerrainSolidFill(QColor newColor);
		void	SetTerrainGradientOutline(QGradientStops newStops);
		void	SetTerrainGradientFill(QGradientStops newStops);


	signals:

		void	Message(QString);
		void	Instructions(QString);
		void	MouseX(float);			/**< Emits the mouse x-coordinate in domain space as the mouse is moved */
		void	MouseY(float);			/**< Emits the mouse y-coordinate in domain space as the mosue is moved */
		void	UndoAvailable(bool);		/**< Emitted when an undo action becomes available or unavailable */
		void	RedoAvailable(bool);		/**< Emitted when a redo action becomes available or unavailable */
		void	SetCursor(const QCursor &);	/**< Emitted when the cursor needs to change */

		/* Node and Element Count Pass Through Signals */
		void	NumNodesDomain(int);		/**< Emitted when the number of nodes in the domain changes */
		void	NumElementsDomain(int);		/**< Emitted when the number of elements in the domain changes */
		void	NumNodesSelected(int);		/**< Emitted when the number of currently selected nodes changes */
		void	NumElementsSelected(int);	/**< Emitted when the number of currently selected elements changes */

		/* Selection Tool Pass-through Signals */
		void	ToolFinishedDrawing();				/**< Emitted when a selection tool has finished drawing */
		void	CircleToolStatsSet(float, float, float);	/**< Emitted when a circle tool circle changes size */
		void	RectangleToolStatsSet(float, float);		/**< Emitted when a rectangle tool rectangle changes size */

		void	BeingDestroyed();	/**< Emitted when the destructor is first called */
		void	EmitMessage(QString);	/**< Emitted any time a text message needs to be passed to the GUI */
		void	UpdateGL();		/**< Emitted any time the OpenGL context needs to be redrawn */

	protected slots:

		void	LoadLayerToGPU();
		void	EnterDisplayMode();

};

#endif // DOMAIN_H
