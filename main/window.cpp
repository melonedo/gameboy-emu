
#include <cstdio>
#include <SDL.h>
#include "threads.h"
#include "../util/byte-type.h"
#include "../video/video.h"

namespace gameboy
{
  pthread_t window_thread;

  SDL_Window *pWindow;
  SDL_Renderer *pRenderer;

  bool init_window();

  void *window_main(void *param)
  {
    begin_init();
    bool success = init_window();
    end_init(success);
    if (!success)
      return NULL;

    while (!program_ended)
    {
      //
      SDL_Delay(1);
    }
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
      "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      window_width, window_height, SDL_WINDOW_SHOWN);
    if(pWindow == NULL)
    {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError() );
      return false;
    }

    //Create renderer for window
    pRenderer = SDL_CreateRenderer( pWindow, -1, SDL_RENDERER_ACCELERATED);
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
};
