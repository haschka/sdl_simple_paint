#include <stdlib.h>                                                             
#include <stdio.h>                                                              
#include <unistd.h>                                                             
#include <sys/types.h>                                                          
#include <sys/stat.h>                                                           
#include <fcntl.h>                                                              
#include <png.h>

int png_to_image(FILE* f, unsigned int** out_image,
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
    row[i] = (png_byte*)malloc(png_get_rowbytes(structure,info));
    if(row[i] == 0) return 3; 
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

int main(int argc, char** argv) {

  unsigned int* image;

  unsigned int width, height;

  int i;
  
  FILE* f = fopen(argv[1],"rb");
  
  png_to_image(f, &image, &width, &height);

  printf("/* \n");
  printf(" * Image of height = %u \n", width);
  printf(" * Image of width  = %u \n", height);
  printf(" */\n");

  printf("unsigned int sidebar[%i] = { \n", width*height);
  for(i=0;i<width*height-1;i++) {
    printf("%u,", image[i]);
      if(i % 10 == 0) printf( "\n");
  }
  printf("%u };", image[width*height-1]);
}
