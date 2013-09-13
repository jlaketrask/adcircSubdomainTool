#ifndef FORT015_H
#define FORT015_H

#include "Domains/Domain.h"
#include "Layers/Actions/ElementState.h"
#include "SubdomainTools/BoundaryFinder.h"

#include <QString>
#include <QDir>

#include <vector>
#include <iostream>
#include <fstream>

class Fort015
{
	public:
		Fort015();
		~Fort015();

		void	SetPath(QString newPath);
		void	SetSubdomains(std::vector<Domain*> newList);
		void	SetApproach(int approach);
		void	SetRecordFrequency(int frequency);

		bool	WriteFort015FullDomain();
		bool	WriteFort015Subdomain();


	private:

		BoundaryFinder boundaryFinder;

		QString	targetPath;
		int	subdomainApproach;
		int	recordFrequency;

		std::vector<Domain*>		subDomains;
		std::vector<unsigned int>	innerBoundaries;
		std::vector<unsigned int>	outerBoundaries;

		bool	ExtractAllInnerBoundaryNodes();
		bool	ExtractAllOuterBoundaryNodes();
};

#endif // FORT015_H