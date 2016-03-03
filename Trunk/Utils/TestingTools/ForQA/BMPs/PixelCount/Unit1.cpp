

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <string.h>
#include <stdio.h>

#include "Filter3x3.cpp"

//---------------------------------------------------------------------------

#pragma argsused

#define FILE_NAME  "test_full5.bmp"

#define BLACK   0
#define YELLOW  1
#define BROWN   2
#define ORANGE  3

#define WIDTH   1632
#define HEIGHT  215

#define FILTER_SIZE          5 //3
#define FILTER_MIDDLE_INDEX  ( int( FILTER_SIZE / 2 ) )

char* image = NULL;

int Width  = 1632;
int Height = 215;

struct file_header {
   unsigned short magic_number;
   unsigned long  bmp_size;
   unsigned short spare_1;
   unsigned short spare_2;
   unsigned long  raw_data_offset;
};

struct DIB_header {
   unsigned long  size_of_header;
   unsigned long  width;
   unsigned long  height;
   unsigned short color_planes;
   unsigned short bits_per_pixel;
   unsigned long  compression_method;
   unsigned long  image_size_raw;
   unsigned long  horizontal_res;
   unsigned long  vertical_res;
   unsigned long  colors_in_palette;
   unsigned long  important_colors;
};

bool ReadImageAttr(FILE* fp)
{

   file_header FileHeader;
   fread( &FileHeader, sizeof(FileHeader), 1, fp);

   if( FileHeader.magic_number != 0x4d42 )
     return false;

   DIB_header DIBHeader;
   fread( &DIBHeader, sizeof(DIBHeader), 1, fp);

   if( DIBHeader.size_of_header != 40 )
     return false;

   if( DIBHeader.bits_per_pixel != 8 )
     return false;

   Width = DIBHeader.width;
   Height = DIBHeader.height;

   fseek(fp, FileHeader.raw_data_offset, SEEK_SET);

   return true;


}

void ReadImage(char* file_name)
{
   char ch;

   FILE *fp = fopen(file_name, "r+b");

   if(fp) {

     if( !ReadImageAttr(fp) )
     {
       printf("File format must be 8BPP BMP");
       exit(0);
     }

     image = (char*)malloc(Width*Height*sizeof(char));

     fread(image, Width, Height, fp);

     fclose(fp);

   }

//   printf("Blacks = %d\nYellows = %d\nOranges = %d\nBrowns = %d\n", colors[BLACK], colors[YELLOW], colors[ORANGE], colors[BROWN]);
}



bool IsLessInModel(long i, long j)
{
  long colors[4] = {0};
  int y, x;

  //count the colors
  for(y = i-FILTER_MIDDLE_INDEX; y <= i+FILTER_MIDDLE_INDEX; y++) {
    for(x = j-FILTER_MIDDLE_INDEX; x <= j+FILTER_MIDDLE_INDEX; x++) {
      colors[image[ y*Width + x ]]++;
    }
  }

  // it can be a "less" inside model, only
  // if we have ANY model neighbors.
  if( colors[YELLOW] == 0 )
    return false;

  // if we have also a support neighbor => not a less in model (edge between model and support)
  if( colors[BROWN] > 0 || colors[ORANGE] > 0 )
    return false;

  // For 5x5 filters
  // if more than half of neighbors are black
  // this is not a less in model
  if( colors[BLACK] >= (FILTER_SIZE * FILTER_SIZE) / 2 )
    return false;

  return true;
}

bool IsLessInSupport(long i, long j)
{
  long colors[4] = {0};
  int y, x;

  //count the colors
  for(y = i-FILTER_MIDDLE_INDEX; y <= i+FILTER_MIDDLE_INDEX; y++) {
    for(x = j-FILTER_MIDDLE_INDEX; x <= j+FILTER_MIDDLE_INDEX; x++) {
      colors[image[y*Width+x]]++;
    }
  }

  // it can be a "less" inside support, only
  // if we have ANY support neighbors.
  if( colors[ORANGE] == 0 && colors[BROWN] == 0 )
    return false;

  // if we have also a model neighbor => not a less in support (edge between model and support)
  if( colors[YELLOW] > 0 )
    return false;

  // For 5x5 filters
  // if more than half of neighbors are black
  // this is not a less in support
  if( colors[BLACK] >= (FILTER_SIZE * FILTER_SIZE) / 2 )
    return false;

  return true;
}

void CountLess()
{
  long Model = 0;
  long LessInModel = 0;
  long Support = 0;
  long LessInSupport = 0;

  for(int i = FILTER_MIDDLE_INDEX; i < Height - FILTER_MIDDLE_INDEX; i++) {
    for(int j = FILTER_MIDDLE_INDEX; j < Width - FILTER_MIDDLE_INDEX; j++) {

      if( image[i*Width+j] == YELLOW )
        Model++;

      if( image[i*Width+j] == ORANGE || image[i*Width+j] == BROWN )
        Support++;

      if( image[i*Width+j] == BLACK ) {

        if( IsLessInModel(i,j) )
          LessInModel++;

        else if( IsLessInSupport(i,j) )
          LessInSupport++;

      }

    }
  }

  float less_in_model_percentage = 0.0f;
  float less_in_model_compared = 0.0f;
  if( LessInModel != 0 && Model != 0 ) {
    less_in_model_percentage = ( 100.0f * LessInModel) / ( LessInModel + Model );
    less_in_model_compared = ( 100.0f * LessInModel) / ( Model );
  }

  float less_in_support_percentage = 0.0f;
  float less_in_support_compared = 0.0f;
  if( LessInSupport != 0 && Support != 0 ) {
    less_in_support_percentage = ( 100.0f * LessInSupport) / ( LessInSupport + Support );
    less_in_support_compared = ( 100.0f * LessInSupport) / ( Support );
  }


  printf("\nModel = %d\
          \n\LessModel = %d\
          \n\LessModel Percentage = %.3f%%\
          \n\LessModel compared to Model = %.3f%%\
          \n\n\Support = %d\n\LessSupport = %d\
          \n\LessSupport Percentage = %.3f%%\
          \n\LessSupport compared to Support = %.3f%%\n",
          Model, LessInModel, less_in_model_percentage, less_in_model_compared,
          Support, LessInSupport, less_in_support_percentage, less_in_support_compared );
}

int main(int argc, char* argv[])
{
   //getc(stdin);

   // EXE BMP
   if(argc == 2) {
     ReadImage(argv[1]);
   }
   else {
     ReadImage(FILE_NAME);
   }

   CountLess();

   printf("\nPress ENTER to exit");
   getc(stdin);

   return 0;
}
//---------------------------------------------------------------------------