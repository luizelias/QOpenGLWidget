#ifndef UIDRAWMANAGER_H
#define UIDRAWMANAGER_H

#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QVector2D>
#include <QVBoxLayout>
#include <QDebug>

namespace mush3d{

class BrushControls : public QWidget
{
    Q_OBJECT
public:
    enum uiType{kRadiusUI, kIntensityUI, kCompare};

public:
    explicit BrushControls(QWidget * parent = 0);

    void setUiType(BrushControls::uiType t);

    void press(int x, int y);
    void drag(int x, int y);
    void release();

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    BrushControls::uiType uitype_;
    QWidget*  parent_;
    int       maxIntensity_;
    int       intensity_;
    int       pressIntensity_;
    QVector2D pressPosition_;
    QVector2D rectanglePos_;
    bool      dragging_;

    int       radius_;
    int       pressRadius_;

    QVector2D dragPosition_;
};

}

#endif // UIDRAWMANAGER_H
