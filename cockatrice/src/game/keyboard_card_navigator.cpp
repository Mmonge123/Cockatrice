#include "keyboard_card_navigator.h"
#include "player/player_logic.h"
#include "board/card_item.h"
#include "board/arrow_item.h"
#include "../../client/settings/cache_settings.h"
#include <QApplication>
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
    if (QApplication::activePopupWidget()) {
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
            if (isArrowModeActive) {
            createTempArrow(newCard);
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

void KeyboardCardNavigator::createTempArrow(CardItem* targetCard) {
    if (!isArrowModeActive || !arrowOriginCard || !targetCard || !playerLogic) return;

    if (previewArrow) {
        delete previewArrow;
        previewArrow = nullptr;
    }

    previewArrow = new ArrowItem(playerLogic, -1, arrowOriginCard, targetCard, Qt::red);
    if (arrowOriginCard->scene()) {
        arrowOriginCard->scene()->addItem(previewArrow);
    }
}
void KeyboardCardNavigator::createArrow(CardItem* targetCard)
{
    if (!isArrowModeActive || !arrowOriginCard || !targetCard || !playerLogic) return;


    if (previewArrow) {
        delete previewArrow;
        previewArrow = nullptr;
    }
    isArrowModeActive = false;


    if (arrowOriginCard == targetCard) {
        arrowOriginCard = nullptr;
        return; 
    }

    ArrowItem::sendCreateArrowCommand(playerLogic, arrowOriginCard, targetCard, Qt::red);
    
    arrowOriginCard = nullptr;
}

// In keyboard_card_navigator.cpp

void KeyboardCardNavigator::startArrowMode(CardItem* originCard)
{
    if (!originCard || !originCard->scene() || !playerLogic) return;
    
    isArrowModeActive = true;
    arrowOriginCard = originCard;

    // Create a temporary visual arrow pointing to itself so the user knows it worked
    createTempArrow(originCard);
    
    qWarning() << "[KeyNav] Arrow mode STARTED.";
}

void KeyboardCardNavigator::cancelArrowMode()
{
    if (previewArrow) {
        delete previewArrow;
        previewArrow = nullptr;
    }
    isArrowModeActive = false;
    arrowOriginCard = nullptr;
    qWarning() << "[KeyNav] Arrow mode CANCELLED.";
}