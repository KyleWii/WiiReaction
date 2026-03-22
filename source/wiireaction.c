#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <grrlib.h>
#include <ogc/lwp_watchdog.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>

#include "NotoSansJP_Regular_ttf.h"

typedef enum {
	STATE_TITLE,
	STATE_SETTINGS,
	STATE_WAITING,
	STATE_GO,
	STATE_RESULT,
	STATE_TOO_EARLY
} GameState;

void draw_screen(GameState state, GRRLIB_ttfFont *font, wchar_t message[], wchar_t result_text[], wchar_t highscore_text[], int highscore, int colorchange_toggle, int darkorlight_toggle) {
	if (state == STATE_TITLE) {
		if (darkorlight_toggle == 0) {
			GRRLIB_FillScreen(RGBA(45, 45, 45, 255));
			GRRLIB_PrintfTTFW(140, 180, font, L"WiiReaction", 40, RGBA (255, 255, 255, 255));
			GRRLIB_PrintfTTFW(140, 240, font, L"Press ⊕ to start the game.", 18, RGBA (255, 255, 255, 255));
			GRRLIB_PrintfTTFW(140, 280, font, L"Press ⊖ to go to settings.", 18, RGBA (255, 255, 255, 255));
		} else {
			GRRLIB_FillScreen(RGBA(235, 235, 235, 255));
			GRRLIB_PrintfTTFW(140, 180, font, L"WiiReaction", 40, RGBA (0, 0, 0, 255));
			GRRLIB_PrintfTTFW(140, 240, font, L"Press ⊕ to start the game.", 18, RGBA (0, 0, 0, 255));
			GRRLIB_PrintfTTFW(140, 280, font, L"Press ⊖ to go to settings.", 18, RGBA (0, 0, 0, 255));
		}
	} else if (state == STATE_SETTINGS) {
		if (darkorlight_toggle == 0) {
			GRRLIB_FillScreen(RGBA(45, 45, 45, 255));
			if (colorchange_toggle == 0) {
				GRRLIB_PrintfTTFW(140, 180, font, L"Press ① to toggle changing color. It is currently off.", 16, RGBA(255, 255, 255, 255));
			} else {
				GRRLIB_PrintfTTFW(140, 180, font, L"Press ① to toggle changing color. It is currently on.", 16, RGBA(255, 255, 255, 255));
			}
			if (darkorlight_toggle == 0) {
				GRRLIB_PrintfTTFW(140, 240, font, L"Press ② to toggle light/dark theme. The theme is currently dark.", 16, RGBA(255, 255, 255, 255));
			} else {
				GRRLIB_PrintfTTFW(140, 240, font, L"Press ② to toggle light/dark theme. The theme is currently light.", 16, RGBA(255, 255, 255, 255));
			}
			GRRLIB_PrintfTTFW(140, 300, font, L"Press Ⓑ to go back to the title screen.", 20, RGBA(255, 255, 255, 255));
		} else {
			GRRLIB_FillScreen(RGBA(235, 235, 235, 255));
			if (colorchange_toggle == 0) {
				GRRLIB_PrintfTTFW(140, 180, font, L"Press ① to toggle changing color. It is currently off.", 16, RGBA(0, 0, 0, 255));
			} else {
				GRRLIB_PrintfTTFW(140, 180, font, L"Press ① to toggle changing color. It is currently on.", 16, RGBA(0, 0, 0, 255));
			}
			if (darkorlight_toggle == 0) {
				GRRLIB_PrintfTTFW(140, 240, font, L"Press ② to toggle light/dark theme. The theme is currently dark.", 16, RGBA(0, 0, 0, 255));
			} else {
				GRRLIB_PrintfTTFW(140, 240, font, L"Press ② to toggle light/dark theme. The theme is currently light.", 16, RGBA(0, 0, 0, 255));
			}
			GRRLIB_PrintfTTFW(140, 300, font, L"Press Ⓑ to go back to the title screen.", 20, RGBA(0, 0, 0, 255));
		}
	} else {
		if (darkorlight_toggle == 0) {
			if (colorchange_toggle == 0 || state != STATE_GO) {
				GRRLIB_FillScreen(RGBA(20, 20, 30, 255));
			} else {
				GRRLIB_FillScreen(RGBA(90, 255, 90, 120));
			}
			GRRLIB_PrintfTTFW(30, 30, font, L"WiiReaction", 36, RGBA(255, 255, 255, 255));
			GRRLIB_PrintfTTFW(140, 180, font, message, 28, RGBA(255, 255, 0, 255));
			GRRLIB_PrintfTTFW(120, 250, font, result_text, 24, RGBA(200, 200, 200, 255));

			GRRLIB_PrintfTTFW(80, 400, font, L"Ⓐ = React   Ⓑ = Restart   HOME = Quit", 20, RGBA(180, 180, 180, 255));
			if (highscore != 0) {
				GRRLIB_PrintfTTFW(80, 450, font, highscore_text, 24, RGBA(180, 180, 180, 255));
			}
		} else {
			if (colorchange_toggle == 0 || state != STATE_GO) {
				GRRLIB_FillScreen(RGBA(245, 245, 245, 255));
			} else {
				GRRLIB_FillScreen(RGBA(140, 255, 140, 255));
			}
			GRRLIB_PrintfTTFW(30, 30, font, L"WiiReaction", 36, RGBA(0, 0, 0, 255));
			GRRLIB_PrintfTTFW(140, 180, font, message, 28, RGBA(0, 0, 255, 255));
			GRRLIB_PrintfTTFW(120, 250, font, result_text, 24, RGBA(50, 50, 50, 255));

			GRRLIB_PrintfTTFW(80, 400, font, L"Ⓐ = React   Ⓑ = Restart   HOME = Quit", 20, RGBA(90, 90, 90, 255));
			if (highscore != 0) {
				GRRLIB_PrintfTTFW(80, 450, font, highscore_text, 24, RGBA(90, 90, 90, 255));
			}
		}
	}
	GRRLIB_Render();
}

void reset_round(GameState *state, int *random_wait_ms, u64 *wait_start_ticks) {
	*state = STATE_WAITING;
	*random_wait_ms = 2000 + (rand() % 3000);
	*wait_start_ticks = gettime();
}

int main(int argc, char **argv) {
	GRRLIB_ttfFont *font = NULL;

	GameState state = STATE_TITLE;

	u64 wait_start_ticks = 0;
	u64 go_start_ticks = 0;
	u64 reaction_ticks = 0;

	int random_wait_ms = 0;
	int reaction_ms = 0;
	int highscore = 0;

	wchar_t message[128];
	wchar_t result_text[128];
	wchar_t highscore_text[128];

	// 0 = Off, 1 = On
	int colorchange_toggle = 0;
	// 0 = Dark, 1 = Light
	int darkorlight_toggle = 0;

	// Init video/input/GRRLIB
	VIDEO_Init();
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_0, 640, 480);

	GRRLIB_Init();

	// Font
	font = GRRLIB_LoadTTF(NotoSansJP_Regular_ttf, NotoSansJP_Regular_ttf_size);
	if (font == NULL) {
		GRRLIB_Exit();
		return 1;
	}

	// Random seed
	srand(time(NULL));

	while (1) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);

		if (pressed & WPAD_BUTTON_HOME) {
			break;
		}

		u64 now = gettime();

		swprintf(highscore_text, 128, L"Highscore: %d", highscore);

		// ----- Game logic -----
		if (state == STATE_TITLE) {
			if (pressed & WPAD_BUTTON_PLUS) {
				reset_round(&state, &random_wait_ms, &wait_start_ticks);
				continue;
			}
			if (pressed & WPAD_BUTTON_MINUS) {
				state = STATE_SETTINGS;
			}
		}

		if (state == STATE_SETTINGS) {
			if (pressed & WPAD_BUTTON_1) {
				if (colorchange_toggle == 0) {
					colorchange_toggle = 1;
				} else {
					colorchange_toggle = 0;
				}
			}
			if (pressed & WPAD_BUTTON_2) {
				if (darkorlight_toggle == 0) {
					darkorlight_toggle = 1;
				} else {
					darkorlight_toggle = 0;
				}
			}
			if (pressed & WPAD_BUTTON_B) {
				state = STATE_TITLE;
			}
		}

		if (state == STATE_WAITING) {
			swprintf(message, 128, L"Wait for it...");
			swprintf(result_text, 128, L"Don't press Ⓐ yet!");

			u32 elapsed_ms = ticks_to_millisecs(now - wait_start_ticks);

			if (pressed & WPAD_BUTTON_A) {
				state = STATE_TOO_EARLY;
			} else if (elapsed_ms >= (u32)random_wait_ms) {
				state = STATE_GO;
				go_start_ticks = gettime();
			}
		}
		else if (state == STATE_GO) {
			swprintf(message, 128, L"Press Ⓐ now!");
			swprintf(result_text, 128, L"");

			if (pressed & WPAD_BUTTON_A) {
				reaction_ticks = gettime() - go_start_ticks;
				reaction_ms = ticks_to_millisecs(reaction_ticks);
				if (highscore == 0) {
					highscore = reaction_ms;
				} else if (reaction_ms < highscore) {
					highscore = reaction_ms;
				}
				state = STATE_RESULT;
			}
		}
		else if (state == STATE_RESULT) {
			swprintf(message, 128, L"Good job!");
			swprintf(result_text, 128, L"Reaction time: %d ms", reaction_ms);
			if (pressed & WPAD_BUTTON_B) {
				reset_round(&state, &random_wait_ms, &wait_start_ticks);
			}
		}
		else if (state == STATE_TOO_EARLY) {
			swprintf(message, 128, L"Too early!");
			swprintf(result_text, 128, L"Press Ⓑ to try again");
			if (pressed & WPAD_BUTTON_B) {
				reset_round(&state, &random_wait_ms, &wait_start_ticks);
			}
		}

		draw_screen(state, font, message, result_text, highscore_text, highscore, colorchange_toggle, darkorlight_toggle);
	}

	GRRLIB_FreeTTF(font);
	GRRLIB_Exit();
	return 0;
}
