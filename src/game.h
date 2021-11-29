
#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <stdbool.h>


// Bools are set if the button has been seen pressed lately.
// Reset them whenever you've dealt with a button press
extern bool BUTTON_DOWN[2];

// Set when a button is first pressed and then released.
// Use it to detect a complete press
extern bool BUTTON_UP[2];


// Call as often as possible. This polls the buttons.
void Buttons();

// Call now and then to keep the game ticking
void Game();

void SetupGame();

#endif /* SRC_GAME_H_ */
