#pragma once

#include <QWidget>

class CircleProgressBar : public QWidget
{
	Q_OBJECT
public:
	explicit CircleProgressBar(QWidget* parent = nullptr);
	void upd(double pp);
	void paintEvent(QPaintEvent*) override;
	int	 heightForWidth(int width) const override;
	bool hasHeightForWidth() const override;

private:
	double p;
};
