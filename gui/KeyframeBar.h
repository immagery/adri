#ifndef KEYFRAMEBAR_H
#define KEYFRAMEBAR_H

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlabel.h>
#include <QtGui/qpixmap.h>
#include <vector>

using namespace std;
using namespace Qt;

class KeyframeBar : public QLabel {
	Q_OBJECT
public:
	vector<float> keyframes;

    KeyframeBar(QWidget * parent = 0);
	void addKeyframe (int f, int max) {
		keyframes.push_back(f * (float)this->width() / max);
		repaint();
	}
    void paintEvent (QPaintEvent* e);
	void addListOfKeyframes(const vector<int>& v, int max) {
		for (int i = 0; i < v.size(); ++i) keyframes.push_back(v[i] * (float)this->width() / max);
	}
};

#endif // KEYFRAMEBAR_H
