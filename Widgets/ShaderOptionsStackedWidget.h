#ifndef SHADEROPTIONSSTACKEDWIDGET_H
#define SHADEROPTIONSSTACKEDWIDGET_H

#include <QStackedWidget>
#include <QColorDialog>
#include <Widgets/ColorWidgets/ColorGradientFrame.h>
#include <Widgets/ColorWidgets/ValueSlider.h>

namespace Ui {
	class ShaderOptionsStackedWidget;
}

class ShaderOptionsStackedWidget : public QStackedWidget
{
		Q_OBJECT
		
	public:
		explicit ShaderOptionsStackedWidget(QWidget *parent = 0);
		~ShaderOptionsStackedWidget();
		
	private:
		Ui::ShaderOptionsStackedWidget *ui;

	protected slots:

		void	colorChanged(const QColor &c);
		void	addCurrentColorToCustomColors();
};

#endif // SHADEROPTIONSSTACKEDWIDGET_H
