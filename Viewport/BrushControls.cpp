#include "BrushControls.h"

#include "Viewport.h"
#include "Brush.h"
#include "Global.h"
#include "MainWindow.h"

#include <QPainterPath>

namespace mush3d{

BrushControls::BrushControls(QWidget *parent) : QWidget(parent), parent_(parent),
    maxIntensity_(300), intensity_(300), dragging_(false), uitype_(BrushControls::kRadiusUI), radius_(80)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);

    Brush::radius2d = radius_;
}

void BrushControls::setUiType(BrushControls::uiType t){uitype_ = t;}

void BrushControls::press(int x, int y)
{
    pressPosition_ = QVector2D(x,y);

    if(uitype_ == BrushControls::kIntensityUI)
    {
        intensity_ = Brush::intensity * 300;
        pressIntensity_ = intensity_;
        int intensityInsideBox = (x-(maxIntensity_/2)) + intensity_;
        int offset = x-intensityInsideBox;
        rectanglePos_.setX(x+offset);
        rectanglePos_.setY(y);
    }
    else if(uitype_ == BrushControls::kRadiusUI)
    {
        radius_ = Brush::radius2d;
        pressRadius_ = radius_;
    }

    dragging_ = true;

    repaint();
}

void BrushControls::drag(int x, int y)
{
    int size = int(x) - int(pressPosition_.x());

    if(uitype_ == BrushControls::kCompare)
    {
        dragPosition_.setX(x);
        dragPosition_.setY(y);
    }
    else if(uitype_ == BrushControls::kIntensityUI)
    {
        int newIntensity = pressIntensity_+size;
        intensity_ = (newIntensity < 0) ? 0 : newIntensity;
        intensity_ = (intensity_ > maxIntensity_) ? maxIntensity_ : intensity_;

        float normalizedIntensity = float(intensity_)/float(maxIntensity_);

        Brush::intensity = normalizedIntensity;
    }
    else if(uitype_ == BrushControls::kRadiusUI)
    {
        int newRadius = pressRadius_+size;
        radius_ = (newRadius < 0) ? 0 : newRadius;

        if(radius_ > Brush::max2dRadius)
            radius_ = Brush::max2dRadius;

        Brush::radius2d = radius_;
    }
}

void BrushControls::release()
{
    dragging_ = false;
    update();
}

void BrushControls::paintEvent(QPaintEvent * event)
{
    if(!dragging_) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 2));

    if(uitype_ == BrushControls::kCompare)
    {
        painter.setPen(QPen(Qt::darkYellow, 2, Qt::DotLine));


        painter.drawLine(dragPosition_.x(), 0, dragPosition_.x(), height());
    }
    else if(uitype_ == BrushControls::kRadiusUI)
    {
        QPoint center(pressPosition_.x(), pressPosition_.y());
        painter.drawEllipse(center, radius_, radius_);
        painter.setPen(QPen(Qt::black, 5));
        painter.drawPoint(center);
    }
    else
    {
        float intensity = Brush::intensity;

        int recX = int(pressPosition_.x()) - pressIntensity_;
        int recY = int(pressPosition_.y())-8;

        QPainterPath border;
        border.addRoundedRect(QRectF(recX, recY, 300, 16), 10, 10);

        QPainterPath miolo;
        miolo.addRoundedRect(QRectF(recX, recY, intensity_, 16), 10, 10);

        painter.fillPath(border, QColor(0,0,0,100));
        painter.drawPath(border);

        if(intensity > 0.0254)
        {
            painter.fillPath(miolo, QColor(224,92,0,200));
            painter.drawPath(miolo);
        }

        QString msg(QString::number(Brush::intensity));

        if(intensity < 1.0)
        {
            msg = msg.leftJustified(msg.indexOf('.')+3, '0');
            msg = msg.left(msg.indexOf('.')+3);
        }

        if(!(intensity > 0.00001f))
        {
            msg = "0";
        }
        painter.drawText(recX, recY-15, QString("Intensity: ")+msg);
    }
}

}
