#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "src/qoir.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

uint8_t print_error(uint8_t flag)
{
  switch(flag)
  {
    case 1:
      printf("Error (%d): Bad file path.\n", flag);
      return 1;
    case 2:
      printf("Error (%d): Incorrect starting bytes.\n", flag);
      return 2;
  }
}

int main(int argc, char **argv)
{
  uint8_t flag = 0; 

  FILE *file = fopen(argv[1], "rb");
  if (file == NULL) return print_error(1);

  qoi_info_t *img_info = read_qoi(file, &flag);

  fclose(file);

  if (img_info == NULL && print_error(flag) != 0) return 1;
  

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
      printf("Failed to initialize the SDL2 library\n");
      return 1;
  }

  SDL_Window *window = SDL_CreateWindow("QOI_RENDERER",
    0, 0,
    SCREEN_WIDTH, SCREEN_HEIGHT, 0
  );

  if(!window)
  {
      printf("Failed to create window\n");
      return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1,
      SDL_RENDERER_ACCELERATED |
      SDL_RENDERER_TARGETTEXTURE);

  SDL_UpdateWindowSurface(window);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  SDL_Texture *qoi_texture = SDL_CreateTexture(
    renderer, 
    SDL_PIXELFORMAT_RGBA32, 
    SDL_TEXTUREACCESS_STREAMING, 
    img_info->height, 
    img_info->width
  ); 

  SDL_Rect img_area = {
    (SCREEN_WIDTH / 2 - (img_info->height) / 2),
    (SCREEN_HEIGHT / 2 - (img_info->width) / 2),
    img_info->height, img_info->width
  };

  SDL_Rect grid_area = {
    0, 0,
    1920, 1920
  };
  
  SDL_SetTextureBlendMode(qoi_texture, SDL_BLENDMODE_BLEND);
  
  SDL_Surface *grid_surface = IMG_Load("grid.qoi");
  SDL_Texture *grid_texture = SDL_CreateTextureFromSurface(renderer, grid_surface);

  int qoi_pitch = img_info->pixels_size;
  int *qoi_texture_pixels = NULL;

  if (SDL_LockTexture(qoi_texture, NULL, (void**) &qoi_texture_pixels, &qoi_pitch) != 0)
  {
    printf("Unable to lock texture.\n");
    return 1;
  }
  else
  {
    memcpy(qoi_texture_pixels, img_info->pixels, img_info->pixels_size);
  }

  bool should_quit = false;
  SDL_Event e;

  bool left_click_hold = false;
  int x = 0, y = 0;
  int lx = 0, ly = 0;

  int offset = 0;

  uint8_t *pixels = malloc(img_info->pixels_size);
  SDL_Rect old_state;

  while (!should_quit) {
    SDL_GetMouseState(&lx, &ly);

    while (SDL_PollEvent(&e) != 0) {
      switch(e.type)
      {
        case SDL_QUIT:
          should_quit = true;
          break;
        case SDL_KEYDOWN:
          float ratio = ((
            ((float) img_area.w + (img_area.w/10)) / (float) img_area.w) > (((float) img_area.h + (img_area.h/10)) / (float) img_area.h)) ?
              ((float) img_area.h + (img_area.h/10)) / ((float) img_area.h) :
              (((float) img_area.w + (img_area.w/10)) / (float) img_area.w);

          memcpy(pixels, img_info->pixels, img_info->pixels_size);

          switch (e.key.keysym.sym) {
            case SDLK_q:
              should_quit = true;
              break;

            case SDLK_o:
              old_state = img_area;

              img_area.w *= ratio;
              img_area.h *= ratio;

              img_area.x -= (img_area.w - old_state.w) / 2;
              img_area.y -= (img_area.h - old_state.h) / 2;

              break;

            case SDLK_p: 
              old_state = img_area;

              img_area.w = img_area.w / ratio;
              img_area.h = img_area.h / ratio;

              img_area.x -= (img_area.w - old_state.w) / 2;
              img_area.y -= (img_area.h - old_state.h) / 2;
              break;

            case SDLK_k:
              offset += (offset < 255) ? 5 : 0;

              for (int i = 0; i < img_info->pixels_size; i++)
              {
                if ((i+1) % 4 == 0) continue;

                if ((int) pixels[i] + offset > 255)
                  pixels[i] = 255;
                else if ((int) pixels[i] + offset < 0)
                  pixels[i] = 0;
                else
                  pixels[i] += offset;
              }

              memcpy(qoi_texture_pixels, pixels, img_info->pixels_size);

              break;

            case SDLK_j:
              offset -= (offset > -255) ? 5 : 0;

              for (int i = 0; i < img_info->pixels_size; i++)
              {
                if ((i+1) % 4 == 0) continue;

                if ((int) pixels[i] + offset > 255)
                  pixels[i] = 255;
                else if ((int) pixels[i] + offset < 0)
                  pixels[i] = 0;
                else
                  pixels[i] += offset;
              }
              
              memcpy(qoi_texture_pixels, pixels, img_info->pixels_size);

              break;
          }

          break;
        case SDL_MOUSEBUTTONDOWN:
          left_click_hold = true;
          break;
        case SDL_MOUSEBUTTONUP:
          left_click_hold = false;
          break;

        case SDL_MOUSEMOTION:
          SDL_GetMouseState(&x, &y);

          if (left_click_hold)
          {
            img_area.x += (x - lx);
            img_area.y += (y - ly);
          }
          break;
      }
    }

    SDL_UnlockTexture(qoi_texture);

    SDL_RenderCopy(renderer, grid_texture, NULL, &grid_area);
    SDL_RenderCopy(renderer, qoi_texture, NULL, &img_area);

    SDL_RenderPresent(renderer);

    SDL_RenderClear(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(qoi_texture);
  SDL_DestroyTexture(qoi_texture);
  SDL_FreeSurface(grid_surface);

  free(img_info->pixels);
  free(img_info);


  SDL_Quit();
  
  return 0;
}
