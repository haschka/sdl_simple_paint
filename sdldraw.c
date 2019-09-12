#include <stdlib.h>                                                             
#include <stdio.h>                                                              
#include <unistd.h>                                                             
#include <sys/types.h>                                                          
#include <sys/stat.h>                                                           
#include <fcntl.h>                                                              
                                                                                
#include <SDL2/SDL.h>                                                           
#define SDL_MAIN_HANDLED                                                        
                                                                                
unsigned int framenumber = 0;


void change_pixel_color_in_image(unsigned int *image,
				 unsigned int width,
				 unsigned int height,
				 unsigned int x,
				 unsigned int y,
				 unsigned char r,
				 unsigned char g,
				 unsigned char b) {
  
  unsigned int* pixel_i;
  unsigned char* pixel_c;

  pixel_i = image+width*y+x;
  pixel_c = (unsigned char*)pixel_i;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    pixel_c[0] = b;
    pixel_c[1] = g;
    pixel_c[2] = r;
    pixel_c[3] = 255;

#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    
    pixel_c[3] = b;
    pixel_c[2] = g;
    pixel_c[1] = r;
    pixel_c[0] = 255;

#endif

}

void color_out_image(unsigned int *image,
		     unsigned int width,
		     unsigned int height,
		     unsigned char r,
		     unsigned char g,
		     unsigned char b) {

  int i;
  unsigned int* pixel_i;
  unsigned char* pixel_c;
  
  for(i=0;i<width*height;i++) {
    pixel_i = image+i;
    pixel_c = (unsigned char*)pixel_i;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    pixel_c[0] = b;
    pixel_c[1] = g;
    pixel_c[2] = r;
    pixel_c[3] = 255;

#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    
    pixel_c[3] = b;
    pixel_c[2] = g;
    pixel_c[1] = r;
    pixel_c[0] = 255;

#endif
  }
} 

int main(int argc, char** argv) {

  unsigned int* image_frame;

  unsigned int width=640, height=480;
  
  SDL_Window *image_window = NULL;
  SDL_Window *tools_window = NULL;
  SDL_Window *colors_window = NULL;
  SDL_Renderer *image_renderer = NULL;
  SDL_Renderer *tools_renderer = NULL;
  SDL_Renderer *colors_renderer = NULL;
  
  SDL_Event event;
  SDL_Texture *image_texture;
  unsigned int pitch;

  int mouse_down = 0;
  
  SDL_SetMainReady();

  image_frame = (unsigned int*)malloc(sizeof(unsigned int)*width*height);
  
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  image_window = SDL_CreateWindow("Image",100, 100, width, height, 0);
  tools_window = SDL_CreateWindow("Tools",10,100, 80, 480, 0);
  colors_window = SDL_CreateWindow("colors",100, 620, 640, 20, 0);
  
#ifdef SOFTWARE_RENDERING
  image_renderer = SDL_CreateRenderer(image_window,-1,SDL_RENDERER_SOFTWARE);
#else
  image_renderer = SDL_CreateRenderer(image_window,-1,SDL_RENDERER_ACCELERATED);
#endif

  image_texture = SDL_CreateTexture(image_renderer, SDL_PIXELFORMAT_ARGB8888,
				    SDL_TEXTUREACCESS_STREAMING,
				    width,height);

  SDL_LockTexture(image_texture,NULL,(void**)&image_frame, &pitch);
  color_out_image(image_frame,width,height,255,255,255);
  SDL_UnlockTexture(image_texture);

  SDL_RenderCopy(image_renderer,image_texture,NULL,NULL);
  
  SDL_RenderPresent(image_renderer);
  
  while(1) {
    
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_QUIT:
	goto finish;
	break;
      case SDL_MOUSEBUTTONDOWN:

	mouse_down = 1;
	break;
	
      case SDL_MOUSEBUTTONUP:
	
	mouse_down = 0;
	break;
      }
    }

    if(mouse_down) {
      SDL_LockTexture(image_texture,NULL,(void**)&image_frame, &pitch);
      change_pixel_color_in_image(image_frame,width,height,
				  event.motion.x,
				  event.motion.y,0,0,0);
      SDL_UnlockTexture(image_texture);
      SDL_RenderCopy(image_renderer,image_texture,NULL,NULL);
    }
    
    SDL_Delay(10);
    SDL_RenderPresent(image_renderer);
  }
 finish:
  return(0);
}
