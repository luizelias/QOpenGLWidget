#ifndef DEFORMERBUTTON_H
#define DEFORMERBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

namespace mush3d {

class DeformerButton : public QPushButton
{
    Q_OBJECT
public:
    explicit DeformerButton(QWidget *parent = nullptr);


protected:
    void mousePressEvent(QMouseEvent *event);

signals:

};

}

#endif // DEFORMERBUTTON_H
