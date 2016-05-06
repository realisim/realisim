#ifndef realisim_sargam_thinLineEdit_h
#define realisim_sargam_thinLineEdit_h

#include <QEvent>
#include <QLineEdit>
class QMouseEvent;
class QPaintEvent;
#include <QPainter>
#include <QWidget>


namespace realisim
{
namespace sargam
{

class ThinLineEdit : public QWidget
{
    Q_OBJECT
public:
    ThinLineEdit(QWidget* = 0);

    void draw(QPainter*);
    QString getText() const {return mpLineEdit->text();}
    void setText(QString);

signals:
    void textChanged();

protected slots:
    void lineEditEditingFinished();

protected:
    ThinLineEdit(const ThinLineEdit&);
    ThinLineEdit& operator=(const ThinLineEdit&);

    enum state{sIdle, sHovered, sEditing};
    virtual void enterEvent(QEvent *) override;
    virtual void leaveEvent(QEvent *) override;
    state getState() const {return mState;}
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;
    void setState(state);
    void updateUi();

    state mState;
    QLineEdit* mpLineEdit;
    QPoint mPosition;
};

}
}

#endif