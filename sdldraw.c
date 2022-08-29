#include <stdlib.h>                                                             
#include <stdio.h>                                                              
#include <unistd.h>                                                             
#include <sys/types.h>                                                          
#include <sys/stat.h>                                                           
#include <fcntl.h>                                                              
#include <png.h>

#include <SDL2/SDL.h>                                                           
#define SDL_MAIN_HANDLED

#include "sidebar_image.h"

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

color* read_palette_from_file(char* fname) {

  FILE*f = fopen(fname,"r");

  int i;
  
  color* p;
  
  if (f == NULL) {
    p = gen_initial_palette();
    printf("Can not open palette file!");
    return(p);
  }

  p = (color*)malloc(sizeof(color)*16);

  for(i = 0; i < 16; i++) {
    if ( 3 != fscanf(f,"%hhu %hhu %hhu",&(p[i].r), &(p[i].g), &(p[i].b))) {
      free(p);
      p = gen_initial_palette();
      printf("Palette file corrupt!\n");
      fclose(f);

      return(p);
    }
  }
  
  fclose(f);
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

static inline int position_in_image_signed(unsigned int width,
					   unsigned int height,
					   int x, int y) {
  return((x > 0 && x < width && y > 0 && y < height));
}

static inline int position_in_image_unsigned(unsigned int width,
					     unsigned int height,
					     unsigned int x,
					     unsigned int y) {
  return((x > 0 && x < width && y > 0 && y < height));
}
  

void draw_diagonal_in_image(unsigned int *image,
			    unsigned int width,
			    unsigned int height,
			    unsigned int x,
			    unsigned int y,
			    unsigned char r,
			    unsigned char g,
			    unsigned char b,
			    int thickness) {

  int i;
  int th_div_2 = thickness/2;

  int pos_x;
  int pos_y;
  
  for(i=-th_div_2;i<(1+th_div_2);i++) {
    pos_x = x+i;
    pos_y = y-i;
    if(position_in_image_signed(width,height,pos_x,pos_y)) {
      unsigned int* pixel_i;
      unsigned char* pixel_c;
      
      pixel_i = image+width*pos_y+pos_x;
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
}

void draw_square_in_image(unsigned int *image,
			  unsigned int width,
			  unsigned int height,
			  unsigned int x,
			  unsigned int y,
			  unsigned char r,
			  unsigned char g,
			  unsigned char b,
			  int thickness) {

  int i,j;

  int th_div_2=thickness/2;
  
  for(i=x-th_div_2;i<x+th_div_2;i++) {
    for(j=y-th_div_2;j<y+th_div_2;j++) {
      if(position_in_image_signed(width,height,i,j)) {
	  
	    unsigned int* pixel_i;
	    unsigned char* pixel_c;
	    
	    pixel_i = image+width*j+i;
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
  }
}		       			    

void change_pixel_color_in_image(unsigned int *image,
				 unsigned int width,
				 unsigned int height,
				 unsigned int x,
				 unsigned int y,
				 unsigned char r,
				 unsigned char g,
				 unsigned char b,
				 int thickness) {
  
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

int get_toolthickness_by_click_on_tools_window(int y_offset) {

  if(y_offset > 460) {
    return(11);
  }
  if(y_offset <= 460 && y_offset > 442) {
    return(7);
  }
  if(y_offset <= 442 && y_offset > 421) {
    return(5);
  }
  if(y_offset <= 421 && y_offset > 396) {
    return(3);
  }
  if(y_offset <= 396 && y_offset > 375) {
    return(1);
  }
  return(5);
}
   
  

void bresenham_line(unsigned int* image, int width, int height,
		    int x, int y, int prev_x, int prev_y, color c,
		    int thickness,
		    void (*app_at_pixel)(unsigned int*,
					 unsigned int,
					 unsigned int,
					 unsigned int,
					 unsigned int,
					 unsigned char,
					 unsigned char,
					 unsigned char,
					 int )) {
  
  int dx,sx;
  int dy,sy;
  
  int err;
  int err_times_two; 
  
  /* simple implementation of Bresenham's Algorithm */
    
  dx = abs(x-prev_x);   
  dy = -abs(y-prev_y);  
  sx = x < prev_x ? 1 : -1;
  sy = y < prev_y ? 1 : -1;
  
  err = dx+dy;          
  err_times_two = 2*err;
  
  while (1) {
    app_at_pixel(image,width,height,x,y,c.r,c.g,c.b,thickness);
    if(x == prev_x && y == prev_y) break;
    err_times_two = 2*err;
    if(err_times_two > dy) {err += dy; x+=sx;}
    if(err_times_two < dx) {err += dx; y+=sy;}
  }
  
}

int png_to_image_frame(FILE* f, unsigned int** out_image,
		       unsigned int* width,
		       unsigned int* height) {

  int i,j;
  
  png_byte color_type, depth;
  png_bytep *row = NULL;
  png_bytep png_pixel;
  
  png_infop info;
  
  png_structp structure = NULL;

  unsigned int* pixel_i;
  unsigned char* pixel_c;

  unsigned int * image;
  
  structure = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				     NULL, NULL, NULL);
  if(!structure) return(1);

  info = png_create_info_struct(structure);

  if(!info) return(2);

  png_init_io(structure, f);
  png_read_info(structure, info);

  width[0] = png_get_image_width(structure, info);
  height[0] = png_get_image_height(structure, info);
  color_type = png_get_color_type(structure, info);
  depth = png_get_bit_depth(structure, info);

  if (depth == 16) {
    png_set_strip_16(structure);
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(structure);
  }

  if (color_type == PNG_COLOR_TYPE_GRAY && depth < 8) {
    png_set_expand_gray_1_2_4_to_8(structure);
  }

  if (png_get_valid(structure, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(structure);
  }

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_PALETTE) {

    png_set_filler(structure, 0xFF, PNG_FILLER_AFTER);
  }

  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(structure);
  }

  png_read_update_info(structure,info);

  row = (png_bytep*)malloc(sizeof(png_bytep)*height[0]);

  if(row == NULL) return 3;
  
  for(i = 0; i < height[0]; i++) {
    row[i] = (png_byte*)malloc((size_t)png_get_rowbytes(structure,info));
    if(row[i] == NULL) return 3;
  }

  image = (unsigned int*)malloc(sizeof(unsigned int)*width[0]*height[0]);

  if(image == NULL) return 3;
  
  png_read_image(structure,row);

  
  for(j = 0; j < height[0]; j++) {
    for(i = 0; i < width[0]; i++) {
      png_pixel = (row[j]+(i*4));
      pixel_i = image+j*width[0]+i;
      pixel_c = (char*)pixel_i;
      
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      
      pixel_c[0] = png_pixel[2];
      pixel_c[1] = png_pixel[1];
      pixel_c[2] = png_pixel[0];
      pixel_c[3] = png_pixel[3];
      
#endif
      
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      
      pixel_c[3] = png_pixel[2];
      pixel_c[2] = png_pixel[1];
      pixel_c[1] = png_pixel[0];
      pixel_c[0] = png_pixel[3];
      
#endif
    }
  }
  
 out_image[0] = image;
 
  for(i = 0; i < height[0]; i++) {
    free(row[i]);
  }
  free(row);
  
  return(0);
} 

void draw_tool_box_image(unsigned int* tool_box_image) {
}  

void diagonal_pen_tool(unsigned int* image, int width, int height,
		       int x, int y, int prev_x, int prev_y,
		       int prev_pos_avail, color c, int thickness) {
  if(prev_pos_avail) {
    bresenham_line(image, width, height, x, y, prev_x, prev_y, c, thickness,
		   &draw_diagonal_in_image );
  } else {
    draw_diagonal_in_image(image,width,height,x,y,c.r,c.g,c.b, thickness);
  }
}

void square_brush_tool(unsigned int* image, int width, int height,
		       int x, int y, int prev_x, int prev_y,
		       int prev_pos_avail, color c, int thickness) {
  if(prev_pos_avail) {
    bresenham_line(image, width, height, x, y, prev_x, prev_y, c, thickness,
		   &draw_square_in_image );
  } else {
    draw_square_in_image(image,width,height,x,y,c.r,c.g,c.b, thickness);
  }
}

void fill_image_tool(unsigned int* image, int width, int height,
		     int x, int y, int prev_x, int prev_y,
		     int prev_pos_avail, color c, int thickness) {
  color_out_image(image, width, height, c.r,c.g,c.b);
}

void sharp_pencil_tool(unsigned int* image, int width, int height,
		       int x, int y, int prev_x, int prev_y,
		       int prev_pos_avail, color c, int thickness) {

  if(prev_pos_avail) {
    bresenham_line(image, width, height, x, y, prev_x, prev_y, c, thickness,
		   &change_pixel_color_in_image );
  } else {
    change_pixel_color_in_image(image,width,height,x,y,c.r,c.g,c.b, thickness);
  }
}


int save_image_to_png(char* fname, unsigned int* image_frame,
		      unsigned int width, unsigned int height) {
  
  int i,j;
  
  png_bytep *row = NULL;
  png_bytep png_pixel;

  FILE* f = fopen(fname,"wb");

  png_structp structure = png_create_write_struct(PNG_LIBPNG_VER_STRING,
						  NULL, NULL, NULL);

  unsigned int* pixel_i;
  unsigned char* pixel_c;
  
  png_infop info;
  
  if (f == NULL ) return 1;
  if (structure == NULL) return 2;

  info = png_create_info_struct(structure);
  
  png_init_io(structure,f);

  png_set_IHDR(structure, info, width, height, 8,
	       PNG_COLOR_TYPE_RGBA,
	       PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT,
	       PNG_FILTER_TYPE_DEFAULT);

  png_write_info(structure, info);

  row = (png_bytep*)malloc(sizeof(png_bytep)*height);

  if(row == NULL) return 3;
  
  for(i = 0; i < height; i++) {
    row[i] = (png_byte*)malloc(sizeof(unsigned int)*width);
    if(row[i] == 0) return 3; 
  }
  
  for(j = 0; j < height; j++) {
    for(i = 0; i < width; i++) {
      png_pixel = (row[j]+(i*4));
      pixel_i = image_frame+j*width+i;
      pixel_c = (char*)pixel_i;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

      png_pixel[2] = pixel_c[0];
      png_pixel[1] = pixel_c[1];
      png_pixel[0] = pixel_c[2];
      png_pixel[3] = pixel_c[3];

#endif
      
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

      png_pixel[2] = pixel_c[3];
      png_pixel[1] = pixel_c[2];
      png_pixel[0] = pixel_c[1];
      png_pixel[3] = pixel_c[0];

#endif
    }
  }

  png_write_image(structure, row);
  png_write_end(structure, NULL);

  for(i = 0; i < height; i++) {
    free(row[i]);
  }
  free(row);

  png_destroy_write_struct(&structure, &info);

  fclose(f);
}

void (*get_tool_by_click_on_tools_window(int y_offset))(unsigned int*,
							int, int, int,
							int, int, int,
							int, color, int) {

  if(y_offset < 185 && y_offset > 150 ) {
    return(&fill_image_tool);
  }
  if(y_offset <= 150 && y_offset > 125) {
    return(&diagonal_pen_tool);
  }
  if(y_offset <= 125 && y_offset > 95) {
    return(&square_brush_tool);
  }
  if(y_offset <= 95 && y_offset > 75) {
    return(&sharp_pencil_tool);
  }
  return(&sharp_pencil_tool);
}
  
int main(int argc, char** argv) {

  unsigned int* image_buffer_from_file;
  
  unsigned int* image_frame;
  unsigned int* colors_frame;
  unsigned int* tools_frame;
  
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
  SDL_Texture *tools_texture;
  int pitch = width*4, color_pitch=640*4, tools_pitch=80*4;

  int mouse_down = 0;

  int prev_pos_x;
  int prev_pos_y;
  int prev_pos_avail;
  
  color selected_color = {0,0,0};
  color* palette = gen_initial_palette();

  int tool_thickness = 5;

  int cycles = 0;

  size_t o_filename_length, w_filename_length;
  FILE* w_file;
  FILE* o_file;

  char* o_filename;
  char* w_filename;

  int image_comes_from_input_image = 0;
  
  char def_w_filename[13] = "paintout.png";
  
  void (*current_tool)(unsigned int*, int, int, int, int, int, int, int, color,
		       int);
  
  current_tool = &diagonal_pen_tool;

  if(argc > 1) {
    if (access( argv[1], F_OK ) == 0) {
      if (access( argv[1], R_OK ) == 0) {
	
	o_filename_length = sizeof(char)*strlen(argv[1])+1;
	o_filename = (char*)malloc(o_filename_length);
	memcpy(o_filename,argv[1],o_filename_length);
	
	if (access( argv[1], W_OK ) == 0) {
	  w_filename = (char*)malloc(o_filename_length);
	  memcpy(w_filename,argv[1],o_filename_length);
	  w_filename_length = o_filename_length;
	} else {
	  if(access( def_w_filename, W_OK)) {
	    printf("Can not write to given filename, will use %s\n",
		   def_w_filename);
	    
	    w_filename_length = sizeof(char)*13;
	    w_filename = (char*)malloc(w_filename_length);
	    memcpy(w_filename,def_w_filename,w_filename_length);
	  } else {
	    printf("Can not write to an image, drawing can not be saved\n");
	  }
	}
	o_file = fopen(o_filename,"rb");
	
	if(png_to_image_frame(o_file, &image_buffer_from_file,
			       &width, &height)) {
	  printf("Could not read input image - exiting early\n");
	  return(1);
	}
	pitch = 4*width;
	image_comes_from_input_image = 1;
      }
    } else {
      w_file = fopen(argv[1],"wb");
      if(!w_file) {
	printf("Can not write to an image, drawing can not be saved\n");
      } else {
	w_filename_length = sizeof(char)*strlen(argv[1])+1;
	w_filename = (char*)malloc(w_filename_length);
	memcpy(w_filename,argv[1],w_filename_length);
	fclose(w_file);
	unlink(argv[1]);
	if (argc == 4 || argc == 5) {
	  sscanf(argv[2],"%u",&width);
	  sscanf(argv[3],"%u",&height);
	}
      }
    }
  }

  if(argc == 1) {
    w_filename_length = sizeof(char)*13;
    w_filename = (char*)malloc(w_filename_length);
    memcpy(w_filename,def_w_filename,w_filename_length);
  }
  
  if(argc == 5) {
    palette = read_palette_from_file(argv[4]);
  } else {
    palette = gen_initial_palette();
  }
  
  SDL_SetMainReady();
  
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  image_window = SDL_CreateWindow("Image",100, 20, width, height, 0);
  tools_window = SDL_CreateWindow("Tools",10, 20, 80, 480, 0);
  colors_window = SDL_CreateWindow("Colors",100, 520, 640, 40, 0);

  
#ifdef SOFTWARE_RENDERING
  image_renderer = SDL_CreateRenderer(image_window,-1,SDL_RENDERER_SOFTWARE);
  colors_renderer = SDL_CreateRenderer(colors_window,-1,SDL_RENDERER_SOFTWARE);
  tools_renderer = SDL_CreateRenderer(tools_window,-1,SDL_RENDERER_SOFTWARE);
#else
  image_renderer = SDL_CreateRenderer(image_window,
				      -1,SDL_RENDERER_ACCELERATED);
  colors_renderer = SDL_CreateRenderer(colors_window,
				      -1,SDL_RENDERER_ACCELERATED);
  tools_renderer = SDL_CreateRenderer(tools_window,-1,
				      SDL_RENDERER_ACCELERATED);
#endif

  image_texture = SDL_CreateTexture(image_renderer, SDL_PIXELFORMAT_ARGB8888,
				    SDL_TEXTUREACCESS_STREAMING,
				    width,height);
  color_texture = SDL_CreateTexture(colors_renderer, SDL_PIXELFORMAT_ARGB8888,
				    SDL_TEXTUREACCESS_STREAMING,
				    640,40);

  tools_texture = SDL_CreateTexture(tools_renderer, SDL_PIXELFORMAT_ARGB8888,
				    SDL_TEXTUREACCESS_STREAMING,
				    80,480);

  image_window_id = SDL_GetWindowID(image_window);
  tools_window_id = SDL_GetWindowID(tools_window);
  colors_window_id = SDL_GetWindowID(colors_window);
  
  SDL_LockTexture(image_texture,NULL,(void**)&image_frame, &pitch);

  if (image_comes_from_input_image) {

    memcpy(image_frame,image_buffer_from_file,
	   sizeof(unsigned int)*width*height);
    
  } else {

    color_out_image(image_frame,width,height,255,255,255);
    
  }
  
  SDL_UnlockTexture(image_texture);

  SDL_RenderCopy(image_renderer,image_texture,NULL,NULL);
  
  SDL_RenderPresent(image_renderer);

  SDL_LockTexture(color_texture,NULL,(void**)&colors_frame, &color_pitch);
  draw_full_color_palette_image(colors_frame,palette);
  SDL_UnlockTexture(color_texture);

  SDL_RenderCopy(colors_renderer,color_texture,NULL,NULL);
  
  SDL_RenderPresent(colors_renderer);

  SDL_LockTexture(tools_texture,NULL,(void**)&tools_frame, &tools_pitch);
  memcpy(tools_frame,tools_image,sizeof(unsigned int)*38400);
  SDL_UnlockTexture(tools_texture);

  SDL_RenderCopy(tools_renderer,tools_texture,NULL,NULL);
  
  SDL_RenderPresent(tools_renderer);
  
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

	if(event.button.windowID == tools_window_id) {
	  if(event.button.y < 184 && event.button.y > 74) {
	    current_tool = get_tool_by_click_on_tools_window(event.button.y);
	  }
	  if(event.button.y < 356 && event.button.y > 324) {

	    printf("Saving Image \n");
	    SDL_LockTexture(image_texture,NULL,(void**)&image_frame, &pitch);
	    save_image_to_png(w_filename, image_frame, width, height);
	    SDL_UnlockTexture(image_texture);
	    
	  }
	  if(event.button.y < 481 && event.button.y > 374) {
	    tool_thickness =
	      get_toolthickness_by_click_on_tools_window(event.button.y);
	  } 
	}
	
	break;
	
      case SDL_MOUSEBUTTONUP:
	
	mouse_down = 0;
	break;

      case SDL_WINDOWEVENT:

	if(event.window.event == SDL_WINDOWEVENT_CLOSE &&
	   event.window.windowID == image_window_id) {
	  goto finish;
	}
	break;
	
      }
      
    }

    if(mouse_down) {
      SDL_GetMouseState(&x, &y);
      /* x = event.motion.x;
	 y = event.motion.y; */
      printf( "%u %u",x,y);
      if(position_in_image_signed(width,height,x,y)) {
	SDL_LockTexture(image_texture,NULL,(void**)&image_frame, &pitch);
	/* SDL_RenderReadPixels(image_renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
	                        image_frame,pitch); */
	(*current_tool)(image_frame, width, height,
		        x, y, prev_pos_x, prev_pos_y,
		        prev_pos_avail, selected_color, tool_thickness);
	
	SDL_UnlockTexture(image_texture);
	SDL_RenderCopy(image_renderer,image_texture,NULL,NULL);
	SDL_RenderPresent(image_renderer);
	prev_pos_avail = 1;
	
      } else {
	/* no previous position available if cursur is went off screen */
	prev_pos_avail = 0; 
      }
      
      prev_pos_x = x;
      prev_pos_y = y;

    }

    if ( cycles == 128 ) {
      SDL_RenderPresent(tools_renderer);
      SDL_RenderPresent(image_renderer);
      SDL_RenderPresent(colors_renderer);
      cycles = 0;
      
    }
    
    SDL_Delay(10);
    cycles++;
  }
 finish:

  SDL_DestroyTexture(image_texture);
  SDL_DestroyTexture(color_texture);
  SDL_DestroyTexture(tools_texture);
  
  SDL_DestroyRenderer(image_renderer);
  SDL_DestroyRenderer(colors_renderer);
  SDL_DestroyRenderer(tools_renderer);

  SDL_DestroyWindow(image_window);
  SDL_DestroyWindow(tools_window);
  SDL_DestroyWindow(colors_window);
  
  free(palette);
  SDL_Quit();
  return(0);
}
