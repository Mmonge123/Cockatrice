/**
 * @file keyboard_card_navigator.h
 * @ingroup GameInput
 * @brief Handles keyboard navigation for selecting cards using arrow keys.
 *
 * Allows players to navigate through hand cards using directional arrow keys.
 * Spatially-aware: finds adjacent cards based on screen position.
 */

#ifndef KEYBOARD_CARD_NAVIGATOR_H
#define KEYBOARD_CARD_NAVIGATOR_H

#include "zones/card_zone_logic.h"

class PlayerLogic;
class CardItem;
class QKeyEvent;

class KeyboardCardNavigator
{
private:
    bool inHand;
    int currentlyHoveredCardIndex = -1; ///< Index of the currently hovered card
    PlayerLogic *playerLogic;
    
    /**
     * @brief Gets hand cards sorted by visual position.
     * @return List of cards sorted by visual order (left-to-right for horizontal, top-to-bottom for vertical).
     */
    CardList getVisuallyOrderedHandCards() const;
    
public:
    KeyboardCardNavigator(PlayerLogic *player = nullptr);
    void setInHand(bool _inHand);
    void switchCardInHand(QKeyEvent *event);
    void setPlayer(PlayerLogic *player);
    /**
     * @brief Validates and resets the hovered card if needed.
     * Call this when hand composition changes.
     */
    void validateHoveredCard();
};

#endif 
