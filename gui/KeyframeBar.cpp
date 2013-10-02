#include "KeyframeBar.h"
#include <QPainter>

KeyframeBar::KeyframeBar(QWidget *parent) : QLabel(parent){
	keyframes = vector<float>(0);
	keyframes.push_back(0);
}

void KeyframeBar::paintEvent(QPaintEvent* e) {
	QPainter painter(this);
	int w = this->width();
	int h = this->height();
    painter.setPen(QPen(QBrush(QColor(0,0,0,180)),1,Qt::DashLine));
    painter.setBrush(QBrush(QColor(255,255,255,120)));
	QRect selectionRect(0,0,w-5,h-5);
    painter.drawRect(selectionRect);

	for (int i = 0; i < keyframes.size(); ++i) {
		painter.setPen(QPen(Qt::red,Qt::SolidPattern));
		painter.drawRect(QRect((int)keyframes[i],0,1,h-5));
	}
}

