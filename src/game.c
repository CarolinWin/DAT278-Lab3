#include <em_gpio.h>
#include <segmentlcd.h>
#include <stdio.h>

#include "game.h"
#include "globals.h"

bool BUTTON_DOWN[2];
bool BUTTON_UP[2];


static struct {
  char player;
  int player_pos;
  int target_pos;
  int score;
  int reward;
  int oumpf;
  char* message;

  enum {
    MESSAGE,
    SCORE,
    PLAY,
    EGG
  } mode;

  int countdown;

} GameState;

void Buttons() {
  bool b0 = !GPIO_PinInGet(gpioPortB, 9);
  bool b1 = !GPIO_PinInGet(gpioPortB, 10);

  if (BUTTON_DOWN[0] && !b0) BUTTON_UP[0] = true;
  if (b0) BUTTON_DOWN[0] = true;

  if (BUTTON_DOWN[1] && !b1) BUTTON_UP[1] = true;
  if (b1) BUTTON_DOWN[1] = true;

}

void moveTarget() {
  while (GameState.player_pos == GameState.target_pos) GameState.target_pos = sysTicks % 7;
}


void SetupGame() {
  BUTTON_DOWN[0] = false;
  BUTTON_DOWN[1] = false;
  BUTTON_UP[0] = false;
  BUTTON_UP[1] = false;

  GameState.player = 'o';
  GameState.player_pos = 2;
  GameState.score = 0;
  GameState.reward = 1;
  GameState.oumpf = 0;
  GameState.message = "WELCOME";
  GameState.mode = MESSAGE;
  GameState.countdown = 10;
  moveTarget();

  // The pins used for buttons
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);
}

// If a button has been pressed, reset it and return 1 or 2.
// Return 0 if no buttons have been pressed
int check_button() {
  for (int b=0; b<2; b++) {
    if (BUTTON_UP[b]) {
      BUTTON_DOWN[b] = false;
      BUTTON_UP[b] = false;
      return b+1;
    }
  }
  return 0;
}


void nextGameState() {
  if (GameState.countdown > 0) return;
  if (GameState.mode == PLAY) return;

  switch (GameState.mode) {
  case MESSAGE:
    GameState.mode = SCORE;
    GameState.countdown = 10;
    break;

  case EGG:
    break;

  case SCORE:
  default:
    GameState.mode = PLAY;
    GameState.countdown = 0;
    break;
  }
}

// Call now and then, updates game state and the display
// Sometimes this is playing, sometimes it is just showing a message.
void Game() {
	// The string shown on the main part of the LCD
	char display[10] = "         ";

	switch (GameState.mode) {
	case SCORE:
		snprintf(display, 10, "%d Pts", GameState.score);
		//IDLE();
		GameState.countdown--;
		break;

	case MESSAGE:
		snprintf(display, 10, "%s", GameState.message);
		// IDLE();
		GameState.countdown--;
		break;

	case PLAY:{
		int btn = check_button();
		if (btn == 1) {
			if (GameState.player_pos <= 1) {
				GameState.player_pos = 0; GameState.oumpf++;
				if (GameState.oumpf >= 10) {
					GameState.mode = EGG;
					GameState.countdown = 10;
				}
			}
			else GameState.player_pos--;
		}

		if (btn == 2 && GameState.player_pos < 6) GameState.player_pos++;

		if (btn > 0 && GameState.player_pos == GameState.target_pos) {
			GameState.score += GameState.reward;

			moveTarget();

			GameState.message = "BRILLANT";
			GameState.mode = MESSAGE;
			GameState.countdown = 10;
		}

		display[GameState.target_pos] = 'X';
		display[GameState.player_pos] = GameState.player;
	}
	break;

	case EGG:
		GameState.oumpf = 0;
		GameState.message = "HACKER";
		GameState.mode = MESSAGE;
		GameState.score += 1336;
		GameState.player = 'O';
		GameState.countdown = 10;
		break;
	default:
		GameState.mode = PLAY;
	}

	nextGameState();

	//IDLE();
	SegmentLCD_Write(display);
}
