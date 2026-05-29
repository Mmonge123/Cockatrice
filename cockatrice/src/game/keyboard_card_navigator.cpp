#include "keyboard_card_navigator.h"
#include "player/player_logic.h"
#include "board/card_item.h"

#include <QKeyEvent>
KeyboardCardNavigator::KeyboardCardNavigator(PlayerLogic *player)
    : inHand(false), currentlyHoveredCardIndex(-1), playerLogic(player)
{
}

void KeyboardCardNavigator::setInHand(bool _inHand)
{
    inHand = _inHand;
}

void KeyboardCardNavigator::switchCardInHand(QKeyEvent *event)
{

    // Don't check inHand flag - it's timing-dependent. Just check if we have valid player and cards.
    if (!playerLogic) {
        qWarning() << "[KeyNav] playerLogic is NULL";
        return;
    }
    
    HandZoneLogic *handZone = playerLogic->getHandZone();
    if (!handZone) {
        qWarning() << "[KeyNav] handZone is NULL";
        return;
    }
    
    const CardList &handCards = handZone->getCards();
    if (handCards.isEmpty()) {
        qWarning() << "[KeyNav] handCards is EMPTY";
        return;
    }
    event->accept();
    qWarning() << "[KeyNav] Hand size=" << handCards.size() << "index=" << currentlyHoveredCardIndex;
    
    // Check if this is an arrow key we care about
    int keyCode = event->key();
    if (keyCode != Qt::Key_Right && keyCode != Qt::Key_Left) {
        return;
    }
    
    // Calculate new index
    int newIndex = currentlyHoveredCardIndex;
    bool isInitial = (currentlyHoveredCardIndex < 0);
    
    if (isInitial) {
        newIndex = 0;
        qWarning() << "[KeyNav] INIT to 0";
    } else {
        // Validate current index is still in bounds
        if (currentlyHoveredCardIndex >= handCards.size()) {
            qWarning() << "[KeyNav] Index out of bounds, reset to 0";
            currentlyHoveredCardIndex = 0;
            newIndex = 0;
        }
        
        // Calculate movement
        if (keyCode == Qt::Key_Right) {
            newIndex = (currentlyHoveredCardIndex + 1) % handCards.size();
        } else {
            newIndex = (currentlyHoveredCardIndex - 1 + handCards.size()) % handCards.size();
        }
    }
    
    // Unhover old card
    if (currentlyHoveredCardIndex >= 0 && currentlyHoveredCardIndex < handCards.size()) {
        CardItem *oldCard = handCards[currentlyHoveredCardIndex];
        if (oldCard) {
            oldCard->setHovered(false);
            // Force update of old card's area
            if (oldCard->scene()) {
                oldCard->scene()->update(oldCard->sceneBoundingRect());
            }
        }
    }
    
    // Update index and hover new card
    currentlyHoveredCardIndex = newIndex;
    if (newIndex >= 0 && newIndex < handCards.size()) {
        CardItem *newCard = handCards[newIndex];
        if (newCard) {
            newCard->setHovered(true);
            newCard->setFocus();
            // Force update of new card's area
            if (newCard->scene()) {
                newCard->scene()->update(newCard->sceneBoundingRect());
            }
            qWarning() << "[KeyNav] SUCCESS - hovering card at index" << newIndex;
        } else {
            qWarning() << "[KeyNav] ERROR - card at index" << newIndex << "is NULL";
        }
    } else {
        qWarning() << "[KeyNav] ERROR - index out of bounds";
    }
}

void KeyboardCardNavigator::setPlayer(PlayerLogic *player)
{
    playerLogic = player;
}


void KeyboardCardNavigator::setCurrentlyHoveredCardIndex(int index)
{
    currentlyHoveredCardIndex = index;
}

void KeyboardCardNavigator::UnhoverCurrentCard()
{
    if (!playerLogic) {
        return;
    }
    
    HandZoneLogic *handZone = playerLogic->getHandZone();
    if (!handZone) {
        return;
    }
    
    const CardList &handCards = handZone->getCards();
    if (currentlyHoveredCardIndex >= 0 && currentlyHoveredCardIndex < handCards.size()) {
        CardItem *currentCard = handCards[currentlyHoveredCardIndex];
        if (currentCard) {
            currentCard->setHovered(false);
            // Force update of current card's area
            if (currentCard->scene()) {
                currentCard->scene()->update(currentCard->sceneBoundingRect());
            }
        }
    }
}