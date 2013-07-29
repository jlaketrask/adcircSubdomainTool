#include "CreationSelectionLayer.h"

/**
 * @brief Constructor initializes all variables to default values
 *
 * Constructor initializes all variables to default values and pointers
 * to zero.
 *
 */
CreationSelectionLayer::CreationSelectionLayer()
{
	activeTool = 1;
	circleTool = 0;

	selectedState = 0;

	glLoaded = false;
	camera = 0;
	VAOId = 0;
	VBOId = 0;
	IBOId = 0;
	outlineShader = 0;
	fillShader = 0;
	boundaryShader = 0;

	mousePressed = false;
	CreateCircleTool();
}


/**
 * @brief Deconstructor that cleans up data on the GPU and any allocated data
 *
 * Cleans up any data that has been created by this class. Marks data for
 * deletion on the GPU. Note that we are not responsible for cleaning up
 * the VBO.
 *
 */
CreationSelectionLayer::~CreationSelectionLayer()
{
	DEBUG("Deleting Creation Selection Layer. Layer ID: " << GetID());

	/* Clean up shaders */
	if (outlineShader)
		delete outlineShader;
	if (fillShader)
		delete fillShader;
	if (boundaryShader)
		delete boundaryShader;

	// Clean up the OpenGL stuff
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/* Note that we aren't responsible for cleaning up the VBO */

	if (VAOId)
		glDeleteBuffers(1, &VAOId);
	if (IBOId)
		glDeleteBuffers(1, &IBOId);

	/* Delete all states */
	if (selectedState)
		delete selectedState;
	ClearUndoStack();
	ClearRedoStack();
}


/**
 * @brief Draws the selected Elements
 *
 * Draws the currently selected Elements (fill and then outline), as well as boundary
 * segments if they are defined. Also draws any tool that is currently in use.
 *
 */
void CreationSelectionLayer::Draw()
{
	if (glLoaded && selectedState)
	{
		glBindVertexArray(VAOId);

		if (fillShader)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (fillShader->Use())
				glDrawElements(GL_TRIANGLES, selectedState->GetState()->size()*3, GL_UNSIGNED_INT, (GLvoid*)0);
		}

		if (outlineShader)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (outlineShader->Use())
				glDrawElements(GL_TRIANGLES, selectedState->GetState()->size()*3, GL_UNSIGNED_INT, (GLvoid*)0);
		}

		// Draw boundaries here
	}

	if (activeTool == CIRCLETOOLINDEX && circleTool)
		circleTool->Draw();
}


/**
 * @brief Loads the currently selected Element data to the GPU
 *
 * Loads the currently selected Element data to the GPU, getting rid of any element
 * data that is already there.
 *
 */
void CreationSelectionLayer::LoadDataToGPU()
{
	/* Make sure we've got all of the necessary Buffer Objects created */
	if (!glLoaded)
		InitializeGL();

	/* Make sure initialization succeeded */
	if (glLoaded && selectedState)
	{
		/* Load the connectivity data (elements) to the GPU, getting rid of any data that's already there */
		std::vector<Element*> *currSelection = selectedState->GetState();
		const size_t IndexBufferSize = 3*sizeof(GLuint)*currSelection->size();
		if (IndexBufferSize && VAOId && IBOId)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize, NULL, GL_STATIC_DRAW);
			GLuint* glElementData = (GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
			if (glElementData)
			{
				Element* currElement;
				int i=0;
				for (std::vector<Element*>::iterator it=currSelection->begin(); it != currSelection->end(); ++it, i++)
				{
					currElement = *it;
					glElementData[3*i+0] = (GLuint)currElement->n1->nodeNumber-1;
					glElementData[3*i+1] = (GLuint)currElement->n2->nodeNumber-1;
					glElementData[3*i+2] = (GLuint)currElement->n3->nodeNumber-1;
				}
			} else {
				glLoaded = false;
				DEBUG("ERROR: Mapping index buffer for Subdomain Creation Selection Layer " << GetID());
				emit emitMessage("<p style:color='red'><strong>Error: Unable to load index data to GPU (Subdomain Creation Selection Layer)</strong>");
				return;
			}

			if (glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) == GL_FALSE)
			{
				glLoaded = false;
				DEBUG("ERROR: Unmapping index buffer for Subdomain Creation Selection Layer " << GetID());
				return;
			}
		}

		GLenum errorCheck = glGetError();
		if (errorCheck == GL_NO_ERROR)
		{
			if (VAOId && VBOId && IBOId)
			{
				glLoaded = true;
			} else {
				DEBUG("Subdomain Creation Selection Layer Data Not Loaded");
			}
		} else {
			const GLubyte *errString = gluErrorString(errorCheck);
			DEBUG("CreationSelectionLayer OpenGL Error: " << errString);
			glLoaded = false;
		}

		emit Refreshed();
		emit NumElementsSelected(currSelection->size());
	}
}


/**
 * @brief Sets the camera used during drawing operations
 *
 * Sets the camera being used during drawing operations
 *
 * @param cam Pointer to the GLCamera object
 */
void CreationSelectionLayer::SetCamera(GLCamera *newCamera)
{
	camera = newCamera;

	/* Set the camera for the shaders */
	if (outlineShader)
		outlineShader->SetCamera(newCamera);
	if (fillShader)
		fillShader->SetCamera(newCamera);
	if (boundaryShader)
		boundaryShader->SetCamera(newCamera);

	/* Set the camera for the tools */
	if (circleTool)
		circleTool->SetCamera(newCamera);
}


/**
 * @brief Returns the number of Elements that are currently selected
 *
 * Returns the number of Elements that are currently selected
 *
 * @return The number of Elements that are currently selected
 */
unsigned int CreationSelectionLayer::GetNumElementsSelected()
{
	if (selectedState)
		return selectedState->GetState()->size();
	return 0;
}


/**
 * @brief Sets the Terrain Layer that all actions and selections will be performed on
 *
 * Sets the Terrain Layer that all actions and selections will be performed on. This is
 * where the selection layer gets the vertex data (VBOId) for drawing.
 *
 * @param newLayer Pointer to the TerrainLayer object
 */
void CreationSelectionLayer::SetTerrainLayer(TerrainLayer *newLayer)
{
	terrainLayer = newLayer;

	connect(terrainLayer, SIGNAL(finishedLoadingToGPU()), this, SLOT(TerrainDataLoaded()));

	if (circleTool)
		circleTool->SetTerrainLayer(newLayer);
}


/**
 * @brief Select the Selection Tool to be used for the next interaction
 *
 * Select the Selection Tool to be used for the next interaction:
 * - 0 - Stop using tools
 * - 1 - CircleTool - Select Elements inside of a circle
 *
 * @param toolID The tool to be used for the next interaction
 */
void CreationSelectionLayer::UseTool(int toolID)
{
	/* Make sure we're trying to select an ID that we've got */
	if (toolID >= 0 && toolID <= AVAILABLETOOLS)
	{
		activeTool = toolID;

		/* If the tool hasn't been created yet, create it now */
		if (activeTool == CIRCLETOOLINDEX && !circleTool)
			circleTool = new CircleTool();
	}
}


/**
 * @brief Passes the mouse click coordinates to the currently active selection tool
 *
 * Passes the mouse click coordinates to the currently active selection tool.
 *
 * @param x x-coordinate (pixels)
 * @param y y-coordinate (pixels)
 */
void CreationSelectionLayer::MouseClick(int x, int y)
{
	mousePressed = true;

	if (activeTool == CIRCLETOOLINDEX && circleTool)
		circleTool->SetCenter(x, y);
}


/**
 * @brief Passes the mouse coordinates to the currently active selection tool when the mouse is moved
 *
 * Passes the mouse coordinates to the currently active selection tool when the mouse is moved
 *
 * @param x x-coordinate (pixels)
 * @param y y-coordinate (pixels)
 */
void CreationSelectionLayer::MouseMove(int x, int y)
{
	if (mousePressed && activeTool == CIRCLETOOLINDEX && circleTool)
		circleTool->SetRadiusPoint(x, y);
}


/**
 * @brief Passes the mouse coordinates to the currently active selection tool when the mouse click is released
 *
 * Passes the mouse coordinates to the currently active selection tool when the mouse click is released
 *
 * @param x x-coordinate (pixels)
 * @param y y-coordinate (pixels)
 */
void CreationSelectionLayer::MouseRelease(int x, int y)
{
	mousePressed = false;

	if (activeTool == CIRCLETOOLINDEX && circleTool)
		circleTool->CircleFinished();
}


/**
 * @brief Tells all of the tools that the size of the OpenGL context has changed
 *
 * Tells all of the tools that the size of the OpenGL context has changed
 *
 * @param w The new OpenGL context width
 * @param h The new OpenGL context height
 */
void CreationSelectionLayer::WindowSizeChanged(float w, float h)
{
	if (circleTool)
		circleTool->SetViewportSize(w, h);
}


/**
 * @brief Undoes the previously performed selection or deselection
 *
 * Undoes the previously performed selection or deselection by reverting
 * to the previous state (the last one pushed onto the undo stack).
 *
 */
void CreationSelectionLayer::Undo()
{
	if (!undoStack.empty() && selectedState)
	{
		redoStack.push(selectedState);
		selectedState = undoStack.top();
		undoStack.pop();
		emit RedoAvailable(true);
		if (undoStack.empty())
			emit UndoAvailable(false);
		LoadDataToGPU();
	}
}


/**
 * @brief Redoes the last undone selection or deselection
 *
 * Redoes the last undone selection or deselection by reverting
 * to the next state on the redo stack.
 *
 */
void CreationSelectionLayer::Redo()
{
	if (!redoStack.empty() && selectedState)
	{
		undoStack.push(selectedState);
		selectedState = redoStack.top();
		redoStack.pop();
		emit UndoAvailable(true);
		if (redoStack.empty())
			emit RedoAvailable(false);
		LoadDataToGPU();
	}
}


/**
 * @brief Initializes the Buffer Objects and Shaders objects necessary for drawing the
 * selection layer
 *
 * This function initializes the Buffer Objects in the OpenGL context as well as the
 * Shader objects necessary for drawing the selection layer. Default transparent grays
 * are used for color.
 *
 * This layer makes use of the vertex data that is already on the GPU from the TerrainLayer.
 * Therefore, we only need to create a new Vertex Array Object and Index Buffer Object.
 *
 */
void CreationSelectionLayer::InitializeGL()
{
	/* Only perform initialization if we have a VBO from a TerrainLayer */
	if (VBOId)
	{
		/* Create new shaders */
		outlineShader = new SolidShader();
		fillShader = new SolidShader();
		boundaryShader = new SolidShader();

		/* Set the shader properties */
		fillShader->SetColor(0.4, 0.4, 0.4, 0.4);
		outlineShader->SetColor(0.2, 0.2, 0.2, 0.2);
		boundaryShader->SetColor(0.0, 0.0, 0.0, 0.8);
		if (camera)
		{
			fillShader->SetCamera(camera);
			outlineShader->SetCamera(camera);
			boundaryShader->SetCamera(camera);
		}

		glGenVertexArrays(1, &VAOId);
		glGenBuffers(1, &IBOId);

		/* Bind the VBO and IBO to the VAO */
		glBindVertexArray(VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, VBOId);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOId);
		glBindVertexArray(0);

		GLenum errorCheck = glGetError();
		if (errorCheck == GL_NO_ERROR)
		{
			if (VAOId && VBOId && IBOId)
			{
				DEBUG("Subdomain Creation Selection Layer Initialized");
				glLoaded = true;
			} else {
				DEBUG("Subdomain Creation Selection Layer Not Initialized");
				glLoaded = false;
			}
		} else {
			const GLubyte *errString = gluErrorString(errorCheck);
			DEBUG("Subdomain Creation Selection Layer OpenGL Error: " << errString);
			glLoaded = false;
		}
	} else {
		DEBUG("Subdomain Creation Selection Layer GL not initialized: TerrainLayer not set");
		glLoaded = false;
	}
}


/**
 * @brief Creates the circle selection tool
 *
 * Creates the circle selection tool. We hook up the signal/slot mechanism here
 * so that this layer knows when the tool has finished finding all elements and
 * is ready to be queried.
 *
 */
void CreationSelectionLayer::CreateCircleTool()
{
	if (!circleTool)
		circleTool = new CircleTool();

	circleTool->SetTerrainLayer(terrainLayer);
	circleTool->SetCamera(camera);
	connect(circleTool, SIGNAL(FinishedSearching()), this, SLOT(CircleToolFinishedSearching()));
}


/**
 * @brief Clears the undo stack
 *
 * Clears the undo stack by deleting all objects it contains
 *
 */
void CreationSelectionLayer::ClearUndoStack()
{
	while(!undoStack.empty())
	{
		ElementState *curr = undoStack.top();
		undoStack.pop();
		if (curr)
			delete curr;
	}
	emit UndoAvailable(false);
}


/**
 * @brief Clears the redo stack
 *
 * Clears the redo stack by deleting all objects it contains
 *
 */
void CreationSelectionLayer::ClearRedoStack()
{
	while(!redoStack.empty())
	{
		ElementState *curr = redoStack.top();
		redoStack.pop();
		if (curr)
			delete curr;
	}
	emit RedoAvailable(false);
}


/**
 * @brief Slot that contains all action that need to be performed after the
 * terrain data has been loaded to the GPU
 *
 * Slot that contains all action that need to be performed after the
 * terrain data has been loaded to the GPU. Queries the terrain layer
 * for its vertex buffer object ID.
 *
 */
void CreationSelectionLayer::TerrainDataLoaded()
{
	VBOId = terrainLayer->GetVBOId();
}


/**
 * @brief Queries the circle tool for currently selected elements
 *
 * Queries the circle tool for currently selected elements. Creates a new list
 * of currently selected elements by combining the currently selected elements
 * with the newly selected ones.
 *
 * The previously selected elements list is pushed onto the undo stack and
 * the new list is made the currently selected elements list.
 *
 */
void CreationSelectionLayer::CircleToolFinishedSearching()
{
	if (!selectedState)
		selectedState = new ElementState();
	if (circleTool)
	{
		/* Create the new state object */
		ElementState *newState = new ElementState(circleTool->GetSelectedElements());

		/* Get pointers to new list of selected elements and current list of selected elements */
		std::vector<Element*> *newList = newState->GetState();
		std::vector<Element*> *currList = selectedState->GetState();

		DEBUG("Found " << newList->size() << " elements");

		if (newList->size() > 0)
		{
			if (currList->size() > 0)
			{
				/* There are currently selected elements, so combine the lists */
				newList->reserve(newList->size() + currList->size());
				newList->insert(newList->end(), currList->begin(), currList->end());

				/* Push the old list onto the undo stack */
				undoStack.push(selectedState);

				/* Sort the new list */
				std::sort(newList->begin(), newList->end());

				/* Get rid of any duplicates in the newly created list */
				std::vector<Element*>::iterator it;
				it = std::unique(newList->begin(), newList->end());
				newList->resize(std::distance(newList->begin(), it));

				/* Set the new state as the current state */
				selectedState = newState;
			} else {
				/* There aren't any currently selected elements, so just go ahead and
				 * make the current state the new state. Push back
				 * the empty state so that the it is saved in the undo stack */
				undoStack.push(selectedState);

				/* Set the new state as the current state */
				selectedState = newState;
			}

			/* Update the data being displayed */
			LoadDataToGPU();

			/* Clear the Redo stack */
			ClearRedoStack();

			/* Let everyone know we can undo this action */
			emit UndoAvailable(true);
		} else {
			/* No elements were selected, so just go ahead and delete the new list */
			delete newState;
		}
	}
}