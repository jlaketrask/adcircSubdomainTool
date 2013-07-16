#ifndef NODEACTION_H
#define NODEACTION_H

#include "adcData.h"
#include "Action.h"
#include "Layers/SelectionLayer.h"
#include <map>


/**
 * @brief A subclass of Action that represents the selection/deselection of Nodes in the
 * SelectionLayer
 */
class NodeAction : public Action
{
	public:

		NodeAction();
		NodeAction(std::map<unsigned int, Node*> cNodes);

		std::map<unsigned int, Node*>	nodes;	/**< The map of all Nodes that are used in this Action */

		virtual void	RedoAction();
		virtual void	UndoAction();
};

#endif // NODEACTION_H
