#include "ThinLineEdit.h"

using namespace realisim;
    using namespace sargam;

namespace
{
    const QString kStyleSheet = "         \
        QLineEdit{                       \
        border: none;                       \
        background: white;                  \
        }                                   \
        \
        QLineEdit::hover{                \
        border: 1px solid gray;             \
        border-radius: 7px;                 \
        background: white;                  \
        padding: -1px;                      \
        }                                   \
        \
        QLineEdit::focus{                \
        border: 1px solid blue;             \
        border-radius: 7px;                 \
        background: white;                  \
        padding: -1px;                      \
        }                                   \
        ";
}

//-----------------------------------------------------------------------------
ThinLineEdit::ThinLineEdit(QWidget* ipParent) : QWidget(ipParent),
mState(sIdle),
mpLineEdit(0)
{
    setMouseTracking(true);

    mpLineEdit = new QLineEdit(ipParent);
    mpLineEdit->setStyleSheet( kStyleSheet );
    mpLineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    mpLineEdit->hide();

    connect( mpLineEdit, SIGNAL( editingFinished() ),
        this, SLOT( lineEditEditingFinished() ) );
}

//-----------------------------------------------------------------------------
void ThinLineEdit::draw(QPainter* p)
{
    p->save();

    QRect r = rect();
    r.translate( pos() );

    p->setRenderHints( p->renderHints() | QPainter::Antialiasing );
    QPen pen = p->pen();
    p->setFont(font());

    switch( getState() )
    {
    case sIdle:
        p->drawText(r, mpLineEdit->text());
    break;
    case sHovered:
        p->drawText(r, mpLineEdit->text());
        p->drawRoundedRect(r.left(), r.top(),
            r.width() - 4, r.height() - 2,
            7, 7);
    break;
    case sEditing:
    break;
    default: break;
    }

    p->restore();
}

//-----------------------------------------------------------------------------
void ThinLineEdit::enterEvent(QEvent *ipE)
{
    setState(sHovered);
    updateUi();
}

//-----------------------------------------------------------------------------
void ThinLineEdit::leaveEvent(QEvent *ipE)
{
    setState(sIdle);
    updateUi();
}

//-----------------------------------------------------------------------------
void ThinLineEdit::lineEditEditingFinished()
{
    if( rect().contains( mapFromGlobal(QCursor::pos()) ) )
    { setState(sHovered); }
    else {setState(sIdle); }

    mpLineEdit->hide();

    emit textChanged();
    updateUi();
}

//-----------------------------------------------------------------------------
void ThinLineEdit::mouseReleaseEvent(QMouseEvent* ipE)
{
    setState(sEditing);
    mpLineEdit->show();
    mpLineEdit->setFocus();
    updateUi();
}

//-----------------------------------------------------------------------------
void ThinLineEdit::paintEvent(QPaintEvent* iPaintEvent)
{
    QPainter p(this);
    draw(&p);
}

//-----------------------------------------------------------------------------
void ThinLineEdit::setState(state iS)
{
    if(iS != getState()) {mState = iS;}
}

//-----------------------------------------------------------------------------
void ThinLineEdit::setText(QString t)
{
    mpLineEdit->setText(t);

    //resize line edit
    QFontMetrics fm( font() );
    QSize newSize( std::max( fm.width( mpLineEdit->text() + "a" ),
        fm.width( "short" ) ), fm.height() );
    
    mpLineEdit->resize( newSize );
    resize( mpLineEdit->size() );

    update();
}

//-----------------------------------------------------------------------------
void ThinLineEdit::updateUi()
{
    //le set font devrait aller dans la surcharge de la methode setFont du widget
    mpLineEdit->setFont( font() );
    mpLineEdit->move( pos() );
    update();
}