#include <stdlib.h>                                                             
#include <stdio.h>                                                              
#include <unistd.h>                                                             
#include <sys/types.h>                                                          
#include <sys/stat.h>                                                           
#include <fcntl.h>                                                              
                                                                                
#include <SDL2/SDL.h>                                                           
#define SDL_MAIN_HANDLED

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} color;

color c_aqua      = {0,127,255};
color c_salmon    = {255,102,102};
color c_flora     = {102,255,102};
color c_lavender  = {204,102,255};
color c_tangerine = {255,127,0};
color c_aluminium = {153,153,153};
color c_mocha     = {127,63,0};
color c_sky       = {102,204,255};
color c_banana    = {255,255,204};
color c_black     = {0, 0, 0};
color c_white     = {255, 255,255};
color c_red       = {255,0,0};
color c_green     = {0,255,0};
color c_blue      = {0,0,255};

color* gen_initial_palette() {
  color* p = (color*)malloc(sizeof(color)*16);

  color grey180 = {180,180,180};
  color grey230 = {230,230,230}; 
    
  p[0] = c_aqua;     
  p[1] = c_salmon;   
  p[2] = c_flora;    
  p[3] = c_lavender; 
  p[4] = c_tangerine;
  p[5] = c_aluminium;
  p[6] = c_mocha;    
  p[7] = c_sky;      
  p[8] = c_banana;   
  p[9] = c_black;    
  p[10] = c_white;    
  p[11] = c_red;      
  p[12] = c_green;    			    
  p[13] = c_blue;
  p[14] = grey180;
  p[15] = grey230;
  return(p);
}

void draw_full_color_palette_image(unsigned int *palette_image,
				   color* palette) {

  int i,j,k ;
  unsigned int start ;

  unsigned int* pixel_i;
  unsigned char* pixel_c;

  color c;
  
  for(i=0;i<16;i++){
    start=i*40;
    c = palette[i];
    for(j=0;j<40;j++) {
      for(k=0;k<40;k++) {
	pixel_i = palette_image+j*640+start+k;
	pixel_c = (unsigned char*)pixel_i;
	
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

	pixel_c[0] = c.b;
	pixel_c[1] = c.g;
	pixel_c[2] = c.r;
	pixel_c[3] = 255;

#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    
	pixel_c[3] = c.b;
	pixel_c[2] = c.g;
	pixel_c[1] = c.r;
	pixel_c[0] = 255;

#endif
      }
    }
  }
}

	

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

color get_color_by_click_on_colors_window(int x_offset, color* palette) {
  return(palette[x_offset/40]);
}

void sharp_pencil_tool(unsigned int* image, int width, int height,
		       int x, int y, int prev_x, int prev_y,
		       int prev_pos_avail, color c) {
  
  int dx,sx;
  int dy,sy;
  
  int err;
  int err_times_two; 

  if(prev_pos_avail) {

    /* simple implementation of Bresenham's Algorithm */
    
    dx = abs(x-prev_x);   
    dy = -abs(y-prev_y);  
    sx = x < prev_x ? 1 : -1;
    sy = y < prev_y ? 1 : -1;

    err = dx+dy;          
    err_times_two = 2*err;
    
    while (1) {
      change_pixel_color_in_image(image,width,height,x,y,c.r,c.g,c.b);
	if(x == prev_x && y == prev_y) break;
      err_times_two = 2*err;
      if(err_times_two > dy) {err += dy; x+=sx;}
      if(err_times_two < dx) {err += dx; y+=sy;}
    }

  } else {
    change_pixel_color_in_image(image,width,height,x,y,c.r,c.g,c.b);
  }
}

    

int main(int argc, char** argv) {

  unsigned int* image_frame;
  unsigned int* colors_frame;
  
  unsigned int width=640, height=480;
  unsigned int x, y;
  
  SDL_Window *image_window = NULL;
  SDL_Window *tools_window = NULL;
  SDL_Window *colors_window = NULL;

  unsigned int image_window_id, tools_window_id, colors_window_id;

  SDL_Renderer *image_renderer = NULL;
  SDL_Renderer *tools_renderer = NULL;
  SDL_Renderer *colors_renderer = NULL;
  
  SDL_Event event;
  SDL_Texture *image_texture;
  SDL_Texture *color_texture;
  unsigned int pitch = width*4, color_pitch=640*4;

  int mouse_down = 0;

  int prev_pos_x;
  int prev_pos_y;
  int prev_pos_avail;
  
  color selected_color = {0,0,0};
  color* palette = gen_initial_palette();

  void (*current_tool)(unsigned int*, int, int, int, int, int, int, int, color);
  current_tool = &sharp_pencil_tool;
  
  SDL_SetMainReady();

  image_frame = (unsigned int*)malloc(sizeof(unsigned int)*width*height);
  colors_frame = (unsigned int*)malloc(sizeof(unsigned int)*640*40);
  
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  image_window = SDL_CreateWindow("Image",100, 20, width, height, 0);
  tools_window = SDL_CreateWindow("Tools",10, 20, 80, 480, 0);
  colors_window = SDL_CreateWindow("Colors",100, 520, 640, 40, 0);

  
#ifdef SOFTWARE_RENDERING
  image_renderer = SDL_CreateRenderer(image_window,-1,SDL_RENDERER_SOFTWARE);
  colors_renderer = SDL_CreateRenderer(colors_window,-1,SDL_RENDERER_SOFTWARE);
#else
  image_renderer = SDL_CreateRenderer(image_window,
				      -1,SDL_RENDERER_ACCELERATED);
  colors_renderer = SDL_CreateRenderer(colors_window,
				      -1,SDL_RENDERER_ACCELERATED);
#endif

  image_texture = SDL_CreateTexture(image_renderer, SDL_PIXELFORMAT_ARGB8888,
				    SDL_TEXTUREACCESS_STREAMING,
				    width,height);
  color_texture = SDL_CreateTexture(colors_renderer, SDL_PIXELFORMAT_ARGB8888,
				    SDL_TEXTUREACCESS_STREAMING,
				    640,40);


  image_window_id = SDL_GetWindowID(image_window);
  tools_window_id = SDL_GetWindowID(tools_window);
  colors_window_id = SDL_GetWindowID(colors_window);
  
  SDL_LockTexture(image_texture,NULL,(void**)&image_frame, &pitch);
  color_out_image(image_frame,width,height,255,255,255);
  SDL_UnlockTexture(image_texture);

  SDL_RenderCopy(image_renderer,image_texture,NULL,NULL);
  
  SDL_RenderPresent(image_renderer);

  SDL_LockTexture(color_texture,NULL,(void**)&colors_frame, &color_pitch);
  draw_full_color_palette_image(colors_frame,palette);
  SDL_UnlockTexture(color_texture);

  SDL_RenderCopy(colors_renderer,color_texture,NULL,NULL);
  
  SDL_RenderPresent(colors_renderer);
  
  while(1) {
    
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_QUIT:
	goto finish;
	break;
      case SDL_MOUSEBUTTONDOWN:

	if(event.button.windowID == image_window_id) {
	  mouse_down = 1;
	  prev_pos_avail = 0;
	}
	
	if(event.button.windowID == colors_window_id) {
	  selected_color = get_color_by_click_on_colors_window(event.button.x,
							       palette);
	}
	break;
	  
      case SDL_MOUSEBUTTONUP:
	
	mouse_down = 0;
	break;
      }
    }

    if(mouse_down) {
      
      x = event.motion.x;
      y = event.motion.y;
      if(x > 0 && x < width && y > 0 && y < height) {
	SDL_LockTexture(image_texture,NULL,(void**)&image_frame, &pitch);

	(*current_tool)(image_frame, width, height,
		        x, y, prev_pos_x, prev_pos_y,
		        prev_pos_avail, selected_color);
	
	SDL_UnlockTexture(image_texture);
	SDL_RenderCopy(image_renderer,image_texture,NULL,NULL);
	prev_pos_avail = 1; 
      } else {
	/* no previous position available if cursur is went off screen */
	prev_pos_avail = 0; 
      }
      prev_pos_x = x;
      prev_pos_y = y;

    }

    
    SDL_Delay(10);
    SDL_RenderPresent(image_renderer);
    SDL_RenderPresent(colors_renderer);
  }
 finish:
  SDL_DestroyWindow(image_window);
  SDL_DestroyWindow(tools_window);
  SDL_DestroyWindow(colors_window);
  return(0);
}
