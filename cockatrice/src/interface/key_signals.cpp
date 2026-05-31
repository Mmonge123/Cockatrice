#include "key_signals.h"

#include <QKeyEvent>

bool KeySignals::eventFilter(QObject * /*object*/, QEvent *event)
{
    QKeyEvent *kevent;

    if (event->type() != QEvent::KeyPress) {
        return false;
    }

    kevent = static_cast<QKeyEvent *>(event);
    switch (kevent->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (kevent->modifiers().testFlag(Qt::AltModifier) && kevent->modifiers().testFlag(Qt::ControlModifier)) {
                emit onCtrlAltEnter();
            } else if (kevent->modifiers() & Qt::ControlModifier) {
                emit onCtrlEnter();
            } else {
                emit onEnter();
            }

            break;
        case Qt::Key_Right:
            qWarning() << "[KeySignals] Qt::Key_Right detected";
            if (kevent->modifiers() & Qt::ShiftModifier) {
                qWarning() << "[KeySignals] Emitting onShiftRight";
                emit onShiftRight();
            } else {
                qWarning() << "[KeySignals] Emitting onRightArrow";
                emit onRightArrow();
                return true;
            }

            break;
        case Qt::Key_Left:
            qWarning() << "[KeySignals] Qt::Key_Left detected";
            if (kevent->modifiers() & Qt::ShiftModifier) {
                qWarning() << "[KeySignals] Emitting onShiftLeft";
                emit onShiftLeft();
            } else {
            emit onRightArrow();
            return true;
        case Qt::Key_Up:
            emit onUpArrow();
            return true;
        case Qt::Key_Down:
            emit onDownArrow();
            return true;
                emit onLeftArrow();
                return true; 
            }

            break;
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            emit onDelete();

            break;
        case Qt::Key_Minus:
            if (kevent->modifiers().testFlag(Qt::AltModifier) && kevent->modifiers().testFlag(Qt::ControlModifier)) {
                emit onCtrlAltMinus();
            }

            break;
        case Qt::Key_Equal:
            if (kevent->modifiers().testFlag(Qt::AltModifier) && kevent->modifiers().testFlag(Qt::ControlModifier)) {
                emit onCtrlAltEqual();
            }

            break;
        case Qt::Key_BracketLeft:
            if (kevent->modifiers().testFlag(Qt::AltModifier) && kevent->modifiers().testFlag(Qt::ControlModifier)) {
                emit onCtrlAltLBracket();
            }

            break;
        case Qt::Key_BracketRight:
            if (kevent->modifiers().testFlag(Qt::AltModifier) && kevent->modifiers().testFlag(Qt::ControlModifier)) {
                emit onCtrlAltRBracket();
            }

            break;
        case Qt::Key_S:
            if (kevent->modifiers() & Qt::ShiftModifier) {
                emit onShiftS();
            }

            break;
        case Qt::Key_C:
            if (kevent->modifiers() & Qt::ControlModifier) {
                emit onCtrlC();
            }

            break;
        default:
            return false;
    }

    return false;
}
