#include "keyboard_card_navigator.h"
#include "player/player_logic.h"
#include "board/card_item.h"
#include "board/arrow_item.h"
#include "../../client/settings/cache_settings.h"
#include <QApplication>
#include <QKeyEvent>
KeyboardCardNavigator::KeyboardCardNavigator(PlayerLogic *player)
    : currentZone(nullptr), currentlyHoveredCardIndex(-1), isArrowModeActive(false), arrowOriginCard(nullptr), previewArrow(nullptr), playerLogic(player)
{
}

void KeyboardCardNavigator::setCurrentZone(CardZoneLogic *zone)
{
    currentZone = zone;
}

void KeyboardCardNavigator::switchCardInZone(QKeyEvent *event)
{
    // Don't check inHand flag - it's timing-dependent. Just check if we have valid player and cards.
    if (!playerLogic) {
        qWarning() << "[KeyNav] playerLogic is NULL";
        return;
    }
    if (!currentZone) {
        qWarning() << "[KeyNav] currentZone is NULL";
        return;
    }
    if (QApplication::activePopupWidget()) {
        return; 
    }
    
    const CardList &zoneCards = currentZone->getCards();
    if (zoneCards.isEmpty()) {
        qWarning() << "[KeyNav] zoneCards is EMPTY";
        return;
    }
    event->accept();
    qWarning() << "[KeyNav] Zone size=" << zoneCards.size() << "index=" << currentlyHoveredCardIndex;
    
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
        if (currentlyHoveredCardIndex >= zoneCards.size()) {
            qWarning() << "[KeyNav] Index out of bounds, reset to 0";
            currentlyHoveredCardIndex = 0;
            newIndex = 0;
        }
        
        // Calculate movement
        if (keyCode == Qt::Key_Right) {
            newIndex = (currentlyHoveredCardIndex + 1) % zoneCards.size();
        } else {
            newIndex = (currentlyHoveredCardIndex - 1 + zoneCards.size()) % zoneCards.size();
        }
    }
    
    // Unhover old card
    if (currentlyHoveredCardIndex >= 0 && currentlyHoveredCardIndex < zoneCards.size()) {
        CardItem *oldCard = zoneCards[currentlyHoveredCardIndex];
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
    if (newIndex >= 0 && newIndex < zoneCards.size()) {
        CardItem *newCard = zoneCards[newIndex];
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
    if (!playerLogic || !currentZone) {
        return;
    }
    
    const CardList &zoneCards = currentZone->getCards();
    if (currentlyHoveredCardIndex >= 0 && currentlyHoveredCardIndex < zoneCards.size()) {
        CardItem *currentCard = zoneCards[currentlyHoveredCardIndex];
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


void KeyboardCardNavigator::switchZone(QKeyEvent *event)
{
    if (!playerLogic) {
        return;
    }
    if (QApplication::activePopupWidget()) {
        return; 
    }
    
    int keyCode = event->key();
    if (keyCode != Qt::Key_Up && keyCode != Qt::Key_Down) {
        return;
    }
    
    event->accept();
    // Build list with only the zones of interest
    QList<CardZoneLogic *> zonesList;
    
    TableZoneLogic *tableZone = playerLogic->getTableZone();
    StackZoneLogic *stackZone = playerLogic->getStackZone();
    HandZoneLogic *handZone = playerLogic->getHandZone();
    
    if (tableZone) zonesList.append(tableZone);
    if (stackZone) zonesList.append(stackZone);
    if (handZone) zonesList.append(handZone);
    
    if (zonesList.isEmpty()) {
        return;
    }
    
    // Find current zone index
    int currentZoneIndex = zonesList.indexOf(currentZone);
    if (currentZoneIndex < 0) {
        // If no current zone, start at first zone
        currentZoneIndex = 0;
    }
    
    // Calculate new zone index
    int newZoneIndex = currentZoneIndex;
    if (keyCode == Qt::Key_Down) {
        newZoneIndex = (currentZoneIndex + 1) % zonesList.size();
    } else if (keyCode == Qt::Key_Up) {
        newZoneIndex = (currentZoneIndex - 1 + zonesList.size()) % zonesList.size();
    }
    
    // Set the new zone
    CardZoneLogic *newZone = zonesList[newZoneIndex];
    setCurrentZone(newZone);
    
    // Reset card index since we're in a new zone
    currentlyHoveredCardIndex = -1;
    
    qWarning() << "[KeyNav] Switched to zone:" << newZone->getName();
}