#ifndef SUBDOMAINCREATOR_H
#define SUBDOMAINCREATOR_H

#include <QString>
#include <QDir>
#include <QProgressBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QProgressDialog>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>

#include "Domains/Domain.h"
#include "SubdomainTools/BoundaryFinder.h"
#include "Projects/IO/FileIO/BNList14.h"


class SubdomainCreator
{
	public:
		SubdomainCreator();
		~SubdomainCreator();

		bool	CreateSubdomain();

		void	SetDomain(Domain *newDomain);
		void	SetProjectPath(QString newProjectPath);
		void	SetSubdomainName(QString newName);

		QString	GetSubdomainName();
		QString	GetFort14Location();
		QString	GetBNListLocation();
		QString	GetPy140Location();
		QString	GetPy141Location();


	private:

		/* Class Variables */
		BoundaryFinder	boundaryFinder;
		ElementState*	currentSelectedState;
		unsigned int	fullNumNodes;
		unsigned int	fullNumElements;

		std::vector<Element*>		selectedElements;
		std::vector<Node*>		selectedNodes;
		std::vector<unsigned int>	boundaryNodes;

		std::map<unsigned int, unsigned int>	oldToNewNodes;
		std::map<unsigned int, unsigned int>	oldToNewElements;

		QString		projectPath;
		QString		targetPath;
		QString		subdomainName;

		QString		fort14Path;
		QString		bnListPath;
		QString		py140Path;
		QString		py141Path;


		/* Create fort.14 steps */
		void	GetAllRequiredData();
		bool	PerformDataValidation();
		bool	WriteFort14File();
		bool	WriteBNListFile();
		bool	WritePy140File();
		bool	WritePy141File();


		/* Helper Functions */
		void	FindUniqueNodes();
		void	FindBoundaryNodes();
		void	MapOldToNewNodes();
		void	MapOldToNewElements();


		/* Validation Functions */
		bool	TestForValidPath();
		bool	TestForSufficientElements();
		bool	TestForSufficientNodes();
		bool	TestForValidBoundary();

		/* Generic Message Boxes */
		void	FileWriteError(QString fileName);

};

#endif // SUBDOMAINCREATOR_H
