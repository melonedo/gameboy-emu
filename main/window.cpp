
#include <cstdio>
#include <SDL.h>
#include "threads.h"
#include "../util/byte-type.h"
#include "../video/video.h"

namespace gameboy
{

  SDL_Window *pWindow;
  SDL_Renderer *pRenderer;

  // Signle byte representing 8 keys
  // 1 for released, 0 for pressed
  byte_t joypad = 0xff;

  enum {
    KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN,
    KEY_A, KEY_B, KEY_SELECT, KEY_START};
  // A, B, SEL, START are mapped to D, F, E, R respectively

  bool init_window();

  void close_window();

  void refresh_screen();

  void refresh_key(int num, bool key_pressed_down);

  void *window_main(void *param)
  {
    bool success = init_window();
    window_init_promise.set_value(success);
    if (!success)
      return NULL;

    while (!program_ended)
    {
      refresh_screen();
      SDL_Event e;
      while (SDL_PollEvent(&e) != 0)
      {
        if (e.type == SDL_QUIT)
        {
          program_ended = true;
          break;
        }
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
          switch (e.key.keysym.sym)
          {
            case SDLK_UP:
            refresh_key(KEY_UP, e.type == SDL_KEYDOWN);
            break;

            case SDLK_DOWN:
            refresh_key(KEY_DOWN, e.type == SDL_KEYDOWN);
            break;

            case SDLK_LEFT:
            refresh_key(KEY_LEFT, e.type == SDL_KEYDOWN);
            break;

            case SDLK_RIGHT:
            refresh_key(KEY_RIGHT, e.type == SDL_KEYDOWN);
            break;

            case SDLK_d:
            refresh_key(KEY_A, e.type == SDL_KEYDOWN);
            break;

            case SDLK_f:
            refresh_key(KEY_B, e.type == SDL_KEYDOWN);
            break;

            case SDLK_e:
            refresh_key(KEY_SELECT, e.type == SDL_KEYDOWN);
            break;

            case SDLK_r:
            refresh_key(KEY_START, e.type == SDL_KEYDOWN);
            break;
          }
        }
      }
      SDL_Delay(15);
    }
    close_window();
    printf("Press enter to terminate console.\n");
    return NULL;
  }

  bool init_window()
  {
    //Initialize SDL
  	if(SDL_Init(SDL_INIT_VIDEO) < 0)
  	{
  		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
  		return false;
  	}

    //Set texture filtering to linear
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
      printf("Warning: Linear texture filtering not enabled!");
    }

    //Create window
    pWindow = SDL_CreateWindow(
      "Gameboy-emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      window_width, window_height, SDL_WINDOW_SHOWN);
    if(pWindow == NULL)
    {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError() );
      return false;
    }

    //Create renderer for window
    pRenderer = SDL_CreateRenderer(pWindow, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(pRenderer == NULL)
    {
      printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
      return false;
    }

    // Set the logical size to that of a gameboy screen
    if (SDL_RenderSetLogicalSize(pRenderer, screen_column_num, screen_row_num))
    {
      printf("Unable to set renderer logical size! SDL Error: %s\n", SDL_GetError());
      return false;
    }

    return true;
  }

  void close_window()
  {
  	//Destroy window
  	SDL_DestroyRenderer(pRenderer);
  	SDL_DestroyWindow(pWindow);
  	pWindow = NULL;
  	pRenderer = NULL;

  	//Quit SDL subsystems
  	SDL_Quit();
  }

  struct renderer_color_t
  {
    Uint8 r, g, b;
  } colors[4] = {
    // These four colors come from bgb
    {224, 248, 208},
    {136, 192, 112},
    { 52, 104,  86},
    {  8,  24,  32}
  };

  void refresh_screen()
  {
    for (int row = 0; row < screen_row_num; row++)
    {
      for (int col = 0; col < screen_column_num; col++)
      {
        color_t c = screen_buf[row][col + 8];
        SDL_SetRenderDrawColor(pRenderer,
          colors[c].r, colors[c].g, colors[c].b, 0xff);
        SDL_RenderDrawPoint(pRenderer, col, row);
      }
    }
    SDL_RenderPresent(pRenderer);
  }

  void refresh_key(int ind, bool key_down)
  {
    // First check for events
    if (key_down && !joypad)
    {
      // Joybad interrupt
      mem_ref(IF) = mem_ref(IF) | (1 << 4);
    }

    byte_t mask = (1 << ind);
    if (key_down)
    {
      joypad &= ~mask;
    }
    else
    {
      joypad |= mask;
    }
    // printf("%x\n", joypad);
  }

  byte_t write_joypad(byte_t val)
  {
    byte_t res = ~0;

    if (~val & (1 << 4))
    {
      res &= joypad | ~0xf;
    }

    if (~val & (1 << 5))
    {
      res &= (joypad >> 4) | ~0xf;
    }

    res &= ~0x30 | val;
    return res;
  }
};
