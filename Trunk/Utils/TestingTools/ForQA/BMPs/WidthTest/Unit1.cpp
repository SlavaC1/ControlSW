

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <string.h>
#include <stdio.h>

//---------------------------------------------------------------------------

#pragma argsused

#define FILE_NAME      "Model1"

#define DEFINES_INI  ".\\Defines.ini"

#define INVALID_VALUE -1

unsigned char* image = NULL;

FILE* out;

int Width  = 0;
int Height = 0;

int AllowedPixelDeviation = 0;
int MachineType = 0;
char type[10] = "Type_";
char type_i[10] = {0};
char type_index[10] = {0};

float X_MMPerPixel = 0;
float Y_MMPerPixel = 0;
int factor = 0;


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

char* ColorStrArr[10] = {
	"Black",  // 0
	"Yellow", // 1
	"Brown",  // 2
	"Orange", // 3
    "Spare",  // 4
    "Green"   // 5
};

char* ColorCode2Str(int color)
{
	switch (color) {
		case 0:
		case 1:
		case 2:
		case 3:
        case 5:
			return ColorStrArr[color];
        default: // -1
			return "None";
	}
}

#define ADHER_FULL    0
#define ADHER_STRONG  1
#define ADHER_MEDIUM  2
#define ADHER_WEAK    3
#define ADHER_ERROR   4

#define BUCKETS  200

char* AdherenceStrArr[5] = {
	"Full",		// 0
	"Strong",	// 1
	"Medium",	// 2
	"Weak",		// 3
	"Error"		// 4
};

bool Init()
{
	MachineType = GetPrivateProfileInt("Machine", "Type", INVALID_VALUE, DEFINES_INI);
	if( MachineType == INVALID_VALUE )
		return false;

	itoa(MachineType, type_index, 10);
	type_i[0] = NULL;
	strcat(type_i, type);
	strcat(type_i, type_index);

	factor = GetPrivateProfileInt(type_i, "factor", INVALID_VALUE, DEFINES_INI);
	if( factor == INVALID_VALUE )
		return false;
	
	X_MMPerPixel = GetPrivateProfileInt(type_i, "X_MMPerPixel", INVALID_VALUE, DEFINES_INI);
	if( X_MMPerPixel == INVALID_VALUE )
		return false;
		
	Y_MMPerPixel = GetPrivateProfileInt(type_i, "Y_MMPerPixel", INVALID_VALUE, DEFINES_INI);
	if( Y_MMPerPixel == INVALID_VALUE )
		return false;

	X_MMPerPixel /= factor;
	Y_MMPerPixel /= factor;
	
	AllowedPixelDeviation = GetPrivateProfileInt("Deviation", "AllowedPixelDeviation", 0, DEFINES_INI);
	if( AllowedPixelDeviation < 0 )
		return false;
	
	return true;
}

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

	 if( DIBHeader.bits_per_pixel != 4 )
		 return false;

	 Width = DIBHeader.width;
	 //Width += 3; // there are 3 zeros at the end of each line
	 Height = DIBHeader.height;

	 fseek(fp, FileHeader.raw_data_offset, SEEK_SET);

	 return true;
}

bool ReadImage(char* file_name)
{
	 char bmp[100] = ".\\";
	 strcat(bmp, file_name );
	 strcat(bmp, ".bmp");

	 FILE *fp = fopen(bmp, "r+b");

	 if(fp) {

		 if( !ReadImageAttr(fp) )
		 {
			 fprintf(out, "<Error desc = \"File format must be 4BPP BMP\" />\n");
			 exit(0);
		 }

		 int size = Width*Height*sizeof(unsigned char);

		 // 4 BPP !!!
		 if( size % 2 != 0 )
			 size++;
		 size /= 2;


		 image = (unsigned char*)malloc(size);

		 fread(image, size, 1, fp);

		 fclose(fp);

		 return true;

	 }

	 return false;
}

// return the value of the 4 bits pixel
unsigned char Image(__int64 i)
{
	if(i%2 != 0)
		return ( image[i/2] & 0x0F );
	else
		return ( ( image[i/2] & 0xF0 ) >> 4 );
	//return image[i];
}

int VTest(int x, int y_from, int y_to, int color, int alt_color)
{
	int counter = 0;
	__int64 i = 0;

	i = (y_from * Width) + x;

	while(y_from >= y_to)
	{
		if( Image(i) == color || Image(i) == alt_color )
			counter++;

		i -= Width;
		y_from--; // the image is being read fliped vertically
	}

	return counter;
}

int HTest(int y, int x_from, int x_to, int color, int alt_color)
{
	int counter = 0;
	__int64 i = 0;

	i = (y * Width) + x_from;

	while(x_from <= x_to)
	{
		if( Image(i) == color || Image(i) == alt_color )
			counter++;

		i++;
		x_from++;
	}

	return counter;
}

bool RunLineTests(char* ini_file_name)
{
	int x = 0;
	int y_from = 0;
	int y_to = 0;
	int y = 0;
	int x_from = 0;
	int x_to = 0;
	int expected = 0;
	int color = 0;
	int alt_color = 0;
	int pixel_counter = 0;

	float mm = 0;
	float mm_expected = 0;

	int i = 1;

	bool PassedAllTests = true;

	char ini[100] = ".\\";
	strcat(ini, ini_file_name);
	strcat(ini, ".ini");

	char line[10] = "line";
	char line_i[10] = {0};
	char str_i[10] = {0};

	while(true) // for line_? tests
	{
		itoa(i, str_i, 10);
		line_i[0] = NULL;
		strcat(line_i, line);
		strcat(line_i, str_i);

		expected = GetPrivateProfileInt(line_i, "expected", INVALID_VALUE, ini);
		color = GetPrivateProfileInt(line_i, "color", INVALID_VALUE, ini);
		alt_color = GetPrivateProfileInt(line_i, "alt_color", INVALID_VALUE, ini);

		x = GetPrivateProfileInt(line_i, "x", INVALID_VALUE, ini);
		if(x != INVALID_VALUE)
		{
			y_from = GetPrivateProfileInt(line_i, "y_from", INVALID_VALUE, ini);
			y_from = Height - 1 - y_from;

			y_to = GetPrivateProfileInt(line_i, "y_to", INVALID_VALUE, ini);
			y_to = Height - 1 - y_to;

			pixel_counter = VTest(x, y_from, y_to, color, alt_color);

			mm = float(pixel_counter) * Y_MMPerPixel;
			mm_expected = float(expected) * Y_MMPerPixel;

			fprintf(out, "<Line num=\"%d\">\n", i);
			fprintf(out, "<Coordinates x = \"%ld\" yFrom = \"%ld\" yTo = \"%ld\" />\n", x, (Height - 1 - y_from), (Height - 1 - y_to) ); //only for displaying the results, convert the height back to user coordinates (instead of image pixel coordinates)
			fprintf(out, "<Scan Color = \"%s\" AlternativeColor = \"%s\" />\n", ColorCode2Str(color), ColorCode2Str(alt_color) );
			fprintf(out, "<Count pixels = \"%ld\" expected = \"%ld\" />\n", pixel_counter, expected);
			fprintf(out, "<Length_Y mm = \"%.03f\" expected = \"%.03f\" />\n", mm, mm_expected);

			//if( pixel_counter != expected )
			if( pixel_counter < expected - AllowedPixelDeviation ||
				pixel_counter > expected + AllowedPixelDeviation )
			{
				fprintf(out, "<Result pass = \"False\" />\n");
				PassedAllTests = false;
			}
			else
			{
				fprintf(out, "<Result pass = \"True\" />\n");
			}

			fprintf(out, "</Line>\n\n");

			i++;
		}

		else  // if not a vertical test, check if it is a horizontal test
		{

			y = GetPrivateProfileInt(line_i, "y", INVALID_VALUE, ini);
			if(y != INVALID_VALUE)
			{
				y = Height - 1 - y;

				x_from = GetPrivateProfileInt(line_i, "x_from", INVALID_VALUE, ini);
				x_to = GetPrivateProfileInt(line_i, "x_to", INVALID_VALUE, ini);

				pixel_counter = HTest(y, x_from, x_to, color, alt_color);

				mm = float(pixel_counter) * X_MMPerPixel;
				mm_expected = float(expected) * X_MMPerPixel;

				fprintf(out, "<Line num=\"%d\">\n", i);
				fprintf(out, "<Coordinates y = \"%ld\" xFrom = \"%ld\" xTo = \"%ld\" />\n", (Height - 1 - y), x_from, x_to);
				fprintf(out, "<Scan Color = \"%s\" AlternativeColor = \"%s\" />\n", ColorCode2Str(color), ColorCode2Str(alt_color) );
				fprintf(out, "<Count pixels = \"%ld\" expected = \"%ld\" />\n", pixel_counter, expected);
				fprintf(out, "<Length_X mm = \"%.03f\" expected = \"%.03f\" />\n", mm, mm_expected);

				//if( pixel_counter != expected )
				if( pixel_counter < expected - AllowedPixelDeviation ||
					pixel_counter > expected + AllowedPixelDeviation )
				{
					fprintf(out, "<Result pass = \"False\" />\n");
					PassedAllTests = false;
				}
				else 
				{
					fprintf(out, "<Result pass = \"True\" />\n");
				}

				fprintf(out, "</Line>\n\n");

				i++;
			}

			else  // this test is also NOT a vertical test, so it means that we finished the tests
			{
				break;
			}
		}
	}

	return PassedAllTests;
}

bool RunAdherenceTests(char* ini_file_name)
{
	int x_from = 0;
	int x_to = 0;
	int y_from = 0;
	int y_to = 0;
	int color = 0;
	int expected = 0;
	int pattern_type = INVALID_VALUE;
	int pixel_counter = 0;
	int ConsecutiveCounter = 0;
	int MaxConsecutive = 0;

	int i = 1;

	bool PassedAllTests = true;
	
	char ini[100] = ".\\";
	strcat(ini, ini_file_name);
	strcat(ini, ".ini");

	char adher[10] = "Adherence";
	char adher_i[10] = {0};
	char str_i[10] = {0};

	while(true) // for Adherence? tests
	{
		itoa(i, str_i, 10);
		adher_i[0] = NULL;
		strcat(adher_i, adher);
		strcat(adher_i, str_i);

		color = GetPrivateProfileInt(adher_i, "color", INVALID_VALUE, ini);

		if(color == INVALID_VALUE)
			break;  // the 'Adherence?' sequence was broken

		x_from = GetPrivateProfileInt(adher_i, "upper_left_x", INVALID_VALUE, ini);
		x_to = GetPrivateProfileInt(adher_i, "lower_right_x", INVALID_VALUE, ini);

		y_from = GetPrivateProfileInt(adher_i, "upper_left_y", INVALID_VALUE, ini);
		y_from = Height - 1 - y_from;

		y_to = GetPrivateProfileInt(adher_i, "lower_right_y", INVALID_VALUE, ini);
		y_to = Height - 1 - y_to;
		
		expected = GetPrivateProfileInt(adher_i, "expected_type", INVALID_VALUE, ini);
		
		fprintf(out, "<Adherence num=\"%d\">\n", i);
		fprintf(out, "<Coordinates UpperLeftX = \"%ld\" UpperLeftY = \"%ld\" LowerRightX = \"%ld\" LowerRightY = \"%ld\" />\n", x_from, (Height - 1 - y_from), x_to, (Height - 1 - y_to) );
		fprintf(out, "<Scan Color = \"%s\" />\n", ColorCode2Str(color) );

		// check for Horizontal lines (indicate weak adherence)
		for( int y = y_from; y >= y_to; y-- )  // we run downwards because the y values are upside-down
		{
			pixel_counter = HTest(y, x_from, x_to, color, INVALID_VALUE);  // -1 = alternate color that does not exist
			
			if (pixel_counter == ( x_to - x_from + 1) )
			{
				pattern_type = ADHER_WEAK;
				fprintf(out, "<Pattern Type = \"%s\" Expected = \"%s\" />\n", AdherenceStrArr[pattern_type], AdherenceStrArr[expected] );
				break;
			}
		}
		
		if( pattern_type == INVALID_VALUE ) // still did not found the pattern
		{
			// check for Vertical lines (Strong or Weak patterns)
			for( int x = x_from; x <= x_to; x++ )
			{
				pixel_counter = VTest(x, y_from, y_to, color, INVALID_VALUE);  // -1 = alternate color that does not exist
				
				if (pixel_counter == ( y_from - y_to + 1) )  // reverse order because the y values are upside-down
				{
					ConsecutiveCounter++;
					if( ConsecutiveCounter > MaxConsecutive )
						MaxConsecutive = ConsecutiveCounter;
				}
				else 
				{
					ConsecutiveCounter = 0;
				}
			}
		
			switch(MaxConsecutive)
			{
				case 0:  // no consecutive lines of support = full adherence
					pattern_type = ADHER_FULL;
					fprintf(out, "<Pattern Type = \"%s\" Expected = \"%s\" />\n", AdherenceStrArr[ADHER_FULL], AdherenceStrArr[expected] );
					break;
				
				case 1:  // 1 consecutive lines of support = strong adherence
					pattern_type = ADHER_STRONG;
					fprintf(out, "<Pattern Type = \"%s\" Expected = \"%s\" />\n", AdherenceStrArr[ADHER_STRONG], AdherenceStrArr[expected] );
					break;
				
				case 2:  // 2 consecutive lines of support = medium adherence
					pattern_type = ADHER_MEDIUM;
					fprintf(out, "<Pattern Type = \"%s\" Expected = \"%s\" />\n", AdherenceStrArr[ADHER_MEDIUM], AdherenceStrArr[expected] );
					break;
					
				default:
					pattern_type = ADHER_ERROR;
					fprintf(out, "<Pattern Type = \"%s\" Expected = \"%s\" />\n", AdherenceStrArr[ADHER_ERROR], AdherenceStrArr[expected] );
					break;
			}
			
		}

		if( pattern_type != expected )
		{
			fprintf(out, "<Result pass = \"False\" />\n");
			PassedAllTests = false;
		}
		else 
		{
			fprintf(out, "<Result pass = \"True\" />\n");
		}

		fprintf(out, "</Adherence>\n\n");

        i++;
		
	}
	
	return PassedAllTests;
}

void Get2FullestBucketsIndex(__int64* Buckets, int &Fullest, int &SecondFullest)
{
	Fullest = 0;
	SecondFullest = 0;
	
	for(int i = 0; i < BUCKETS; i++)
	{
		if( Buckets[i] > Buckets[Fullest] )
		{
			SecondFullest = Fullest;
			Fullest = i; 
		}
	}
}

int GetMin(int a, int b)
{
	return (a < b) ? a : b ;
}

int GetMax(int a, int b)
{
	return (a > b) ? a : b ;
}

bool RunGridTests(char* ini_file_name)
{
	int ConsecutiveCounter = 0;
	bool PassedAllTests = true;
	
	int grid_size_x = 0;
	int grid_size_y = 0;
	int fine_grid_size_x = 0;
	int fine_grid_size_y = 0;
	int grid_spacing_x = 0;
	int grid_spacing_y = 0;

	float grid_size_x_mm = 0;
	float grid_size_y_mm = 0;
	float fine_grid_size_x_mm = 0;
	float fine_grid_size_y_mm = 0;
	float grid_spacing_x_mm = 0;
	float grid_spacing_y_mm = 0;

	int expected_grid_size_x = 0;
	int expected_grid_size_y = 0;
	int expected_fine_grid_size_x = 0;
	int expected_fine_grid_size_y = 0;
	int expected_grid_spacing_x = 0;
	int expected_grid_spacing_y = 0;

	
	//Bucket sorts
	__int64 BucketsX[BUCKETS] = {0};
	__int64 BucketsY[BUCKETS] = {0};
	
	__int64 i = 0;
	
	char ini[100] = ".\\";
	strcat(ini, ini_file_name);
	strcat(ini, ".ini");

	int color = GetPrivateProfileInt("Grid", "Color", INVALID_VALUE, ini);

	if(color == INVALID_VALUE) // this will indicate that we do not have a "Grid" section
	{
		return true;
	}

    expected_grid_size_x = GetPrivateProfileInt("Grid", "GridSizeX", INVALID_VALUE, ini);
    expected_grid_size_y = GetPrivateProfileInt("Grid", "GridSizeY", INVALID_VALUE, ini);
    expected_grid_spacing_x = GetPrivateProfileInt("Grid", "GridSpacingX", INVALID_VALUE, ini);
    expected_grid_spacing_y = GetPrivateProfileInt("Grid", "GridSpacingY", INVALID_VALUE, ini);
    expected_fine_grid_size_x = GetPrivateProfileInt("Grid", "FineGridSizeX", INVALID_VALUE, ini);
    expected_fine_grid_size_y = GetPrivateProfileInt("Grid", "FineGridSizeY", INVALID_VALUE, ini);

    fprintf(out, "<GridMeasurements>\n", i);

	// check Horizontally
	for( int y = 0; y < Height - 1; y++ )
	{
		// count the grid width
		
		ConsecutiveCounter = 0;

		i = (y * Width);

		for(int x = 0; x < Width; x++)
		{
			if( Image(i) == color )
				ConsecutiveCounter++;
			else
			{
				if( ConsecutiveCounter != 0 )
				{
					BucketsX[ConsecutiveCounter]++;
					ConsecutiveCounter = 0;
				}
			}
			
			i++;
				
		}
		
	}
	
	// check Vertically
	for( int x = 0; x < Width - 1; x++ )
	{
		// count the grid height
		
		ConsecutiveCounter = 0;

		i = x;

		for(int y = 0; y < Height; y++)
		{
			if( Image(i) == color )
				ConsecutiveCounter++;
			else
			{
				if( ConsecutiveCounter != 0 )
				{
					BucketsY[ConsecutiveCounter]++;
					ConsecutiveCounter = 0;
                }
			}
			
			i += Width;
				
		}
		
	}
	
	fprintf(out, "<Scan Color = \"%s\" />\n", ColorCode2Str(color) );

	int Fullest = 0, SecondFullest = 0;

	Get2FullestBucketsIndex(BucketsX, Fullest, SecondFullest);
	fine_grid_size_x = GetMin(Fullest, SecondFullest);
	grid_size_x = GetMax(Fullest, SecondFullest);
	
	Get2FullestBucketsIndex(BucketsY, Fullest, SecondFullest);
	fine_grid_size_y = GetMin(Fullest, SecondFullest);
	grid_size_y = GetMax(Fullest, SecondFullest);

	if( grid_size_x == 0 && fine_grid_size_x == 0 ) //if neither of the grid types have no dimentions == grid not found
	{
		fprintf(out, "<Error Desc = \"Could not find any grid pattern\" />\n" );
		fprintf(out, "<Result pass = \"False\" />\n");
		fprintf(out, "</GridMeasurements>\n\n");
		return false;
	}

    grid_size_x_mm = float(grid_size_x) * X_MMPerPixel;
    grid_size_y_mm = float(grid_size_y) * Y_MMPerPixel;
    fine_grid_size_x_mm = float(fine_grid_size_x) * X_MMPerPixel;
    fine_grid_size_y_mm = float(fine_grid_size_y) * Y_MMPerPixel;

	fprintf(out, "<Grid SizeX = \"%d\" SizeY = \"%d\" SizeX_mm = \"%.03f\" SizeY_mm = \"%.03f\" ExpectedSizeX = \"%d\" ExpectedSizeY = \"%d\"/>\n", grid_size_x, grid_size_y, grid_size_x_mm, grid_size_y_mm, expected_grid_size_x, expected_grid_size_y);
	fprintf(out, "<FineGrid SizeX = \"%d\" SizeY = \"%d\" SizeX_mm = \"%.03f\" SizeY_mm = \"%.03f\" ExpectedSizeX = \"%d\" ExpectedSizeY = \"%d\"/>\n", fine_grid_size_x, fine_grid_size_y, fine_grid_size_x_mm, fine_grid_size_y_mm, expected_fine_grid_size_x, expected_fine_grid_size_y);

	// now scan to measure the spaces between the grid particles
	
	// check Horizontally
	bool Passed1 = false;
	int SpaceConsecutiveCounter = 0;
	for( int y = 0; y < Height - 1; y++ )
	{
		ConsecutiveCounter = 0;
		i = (y * Width);

		for(int x = 0; x < Width; x++) {
			if( Image(i) == color )
				ConsecutiveCounter++;
			else {
				if( ConsecutiveCounter != 0 ) {
				
					// check to verify that we just passed a full grid particle
					if( ConsecutiveCounter == grid_size_x )
					{
						if( !Passed1 ) { // just passing a full grid particle for the first time
							Passed1 = true; //mark it so we can count the spcaes
							SpaceConsecutiveCounter++;
						}
						else { // just passing a full grid particle for the second time
							grid_spacing_x = SpaceConsecutiveCounter;
							break;
						}							
					}
					else { // as long as we did not just pass a full grid particle
						Passed1 = false;
						SpaceConsecutiveCounter = 0;
					}
					
					ConsecutiveCounter = 0;
				}
				else {
					if( Passed1 )
						SpaceConsecutiveCounter++;
				}
			}
			i++;
		}
		
		if(grid_spacing_x)
			break;
	}
	
	// check Vertically
	Passed1 = false;
	SpaceConsecutiveCounter = 0;
	for( int x = 0; x < Width - 1; x++ )
	{
		ConsecutiveCounter = 0;
		i = x;

		for(int y = 0; y < Height; y++) {
			if( Image(i) == color )
				ConsecutiveCounter++;
			else {
				if( ConsecutiveCounter != 0 ) {
				
					// check to verify that we just passed a full grid particle
					if( ConsecutiveCounter == grid_size_y )
					{
						if( !Passed1 ) { // just passing a full grid particle for the first time
							Passed1 = true; //mark it so we can count the spcaes
							SpaceConsecutiveCounter++;
						}
						else { // just passing a full grid particle for the second time
							grid_spacing_y = SpaceConsecutiveCounter;
							break;
						}							
					}
					else { // as long as we did not just pass a full grid particle
						Passed1 = false;
						SpaceConsecutiveCounter = 0;
					}
					
					ConsecutiveCounter = 0;
				}
				else {
					if( Passed1 )
						SpaceConsecutiveCounter++;
				}
			}
			i += Width;
		}
		
		if(grid_spacing_y)
			break;
	}

    grid_spacing_x_mm = float(grid_spacing_x) * X_MMPerPixel;
    grid_spacing_y_mm = float(grid_spacing_y) * Y_MMPerPixel;

	fprintf(out, "<Grid SpacingX = \"%d\" SpacingY = \"%d\" SpacingX_mm = \"%.03f\" SpacingY_mm = \"%.03f\" ExpectedSpacingX = \"%d\" ExpectedSpacingY = \"%d\"/>\n", grid_spacing_x, grid_spacing_y, grid_spacing_x_mm, grid_spacing_y_mm, expected_grid_spacing_x, expected_grid_spacing_y);


    // now compare the expected results and determine the pass result

	if( grid_size_x 		!= 	expected_grid_size_x 		||
		grid_size_y 		!= 	expected_grid_size_y 		||
		fine_grid_size_x	!= 	expected_fine_grid_size_x 	||
		fine_grid_size_y 	!= 	expected_fine_grid_size_y 	||
		grid_spacing_x 		!= 	expected_grid_spacing_x 	||
		grid_spacing_y 		!= 	expected_grid_spacing_y )

		PassedAllTests = false;

	if(PassedAllTests)
    	fprintf(out, "<Result pass = \"True\" />\n");
    else
    	fprintf(out, "<Result pass = \"False\" />\n");
	fprintf(out, "</GridMeasurements>\n\n");
		
	return PassedAllTests;
}

bool RunHollowGridTests(char* ini_file_name)
{
	int ConsecutiveCounter = 0;
	bool PassedAllTests = true;
	
	int grid_size_x = 0;
	int grid_size_y = 0;
	int grid_spacing_x = 0;
	int grid_spacing_y = 0;

	float grid_size_x_mm = 0;
	float grid_size_y_mm = 0;
	float grid_spacing_x_mm = 0;
	float grid_spacing_y_mm = 0;

	int expected_grid_size_x = 0;
	int expected_grid_size_y = 0;
	int expected_grid_spacing_x = 0;
	int expected_grid_spacing_y = 0;

	
	//Bucket sorts
	__int64 BucketsX[BUCKETS] = {0};
	__int64 BucketsY[BUCKETS] = {0};
	
	int x_from = (Width/3);
	int x_to = ((Width*2)/3);
	int y_from = (Height/3);
	int y_to = ((Height*2)/3);
	
	__int64 i = 0;
	
	char ini[100] = ".\\";
	strcat(ini, ini_file_name);
	strcat(ini, ".ini");

	int color = GetPrivateProfileInt("HollowGrid", "Color", INVALID_VALUE, ini);

	if(color == INVALID_VALUE) // this will indicate that we do not have a "Grid" section
	{
		return true;
	}

    expected_grid_size_x = GetPrivateProfileInt("HollowGrid", "GridSizeX", INVALID_VALUE, ini);
    expected_grid_size_y = GetPrivateProfileInt("HollowGrid", "GridSizeY", INVALID_VALUE, ini);
    expected_grid_spacing_x = GetPrivateProfileInt("HollowGrid", "GridSpacingX", INVALID_VALUE, ini);
    expected_grid_spacing_y = GetPrivateProfileInt("HollowGrid", "GridSpacingY", INVALID_VALUE, ini);

    fprintf(out, "<HollowGrid>\n", i);

	// check Horizontally - scan only the inner 1/3rd of the image
	for( int y = y_from ; y < y_to; y++ )
	{
		// count the hollow grid width
		
		ConsecutiveCounter = 0;

		i = (y * Width) + (Width/3);

		for(int x = x_from; x < x_to; x++) // scan only the inner 1/3rd of the image
		{
			if( Image(i) == color )
				ConsecutiveCounter++;
			else
			{
				if( ConsecutiveCounter != 0 )
				{
					BucketsX[ConsecutiveCounter]++;
					ConsecutiveCounter = 0;
				}
			}

			i++;
				
		}
		
	}
	
	// check Vertically
	for( int x = x_from; x < x_to; x++ ) //scan only the inner 1/3rd of the image
	{
		// count the grid height
		
		ConsecutiveCounter = 0;

		i = (Width*(Height/3)) + x;

		for(int y = y_from; y < y_to; y++) //scan only the inner 1/3rd of the image
		{
			if( Image(i) == color )
				ConsecutiveCounter++;
			else
			{
				if( ConsecutiveCounter != 0 )
				{
					BucketsY[ConsecutiveCounter]++;
					ConsecutiveCounter = 0;
                }
			}
			
			i += Width;
				
		}
		
	}
	
	fprintf(out, "<Scan Color = \"%s\" />\n", ColorCode2Str(color) );

	int Fullest = 0, SecondFullest = 0;

	Get2FullestBucketsIndex(BucketsX, Fullest, SecondFullest);
	grid_size_x = GetMax(Fullest, SecondFullest);

	Get2FullestBucketsIndex(BucketsY, Fullest, SecondFullest);
	grid_size_y = GetMax(Fullest, SecondFullest);

	if( grid_size_x == 0 ) //if neither of the grid types have no dimentions == grid not found
	{
		fprintf(out, "<Error Desc = \"Could not find any grid pattern\" />\n" );
		fprintf(out, "<Result pass = \"False\" />\n");
		fprintf(out, "</HollowGridMeasurements>\n\n");
		return false;
	}

    grid_size_x_mm = float(grid_size_x) * X_MMPerPixel;
    grid_size_y_mm = float(grid_size_y) * Y_MMPerPixel;

	fprintf(out, "<Grid SizeX = \"%d\" SizeY = \"%d\" SizeX_mm = \"%.03f\" SizeY_mm = \"%.03f\" ExpectedSizeX = \"%d\" ExpectedSizeY = \"%d\"/>\n", grid_size_x, grid_size_y, grid_size_x_mm, grid_size_y_mm, expected_grid_size_x, expected_grid_size_y);

	// now scan to measure the spaces between the grid particles
	
	// check Horizontally
	bool Passed1 = false;
	int SpaceConsecutiveCounter = 0;
	for( int y = y_from; y < y_to; y++ ) // scan only the inner 1/3rd of the image
	{
		ConsecutiveCounter = 0;
		i = (y * Width) + (Width/3);

		for(int x = x_from; x < x_to; x++) // scan only the inner 1/3rd of the image
        {
			if( Image(i) == color )
				ConsecutiveCounter++;
			else {
				if( ConsecutiveCounter != 0 ) {
				
					// check to verify that we just passed a full grid particle
					if( ConsecutiveCounter == grid_size_x )
					{
						if( !Passed1 ) { // just passing a full grid particle for the first time
							Passed1 = true; //mark it so we can count the spcaes
							SpaceConsecutiveCounter++;
						}
						else { // just passing a full grid particle for the second time
							grid_spacing_x = SpaceConsecutiveCounter;
							break;
						}							
					}
					else { // as long as we did not just pass a full grid particle
						Passed1 = false;
						SpaceConsecutiveCounter = 0;
					}
					
					ConsecutiveCounter = 0;
				}
				else {
					if( Passed1 )
						SpaceConsecutiveCounter++;
				}
			}
			i++;
		}
		
		if(grid_spacing_x)
			break;
	}
	
	// check Vertically
	Passed1 = false;
	SpaceConsecutiveCounter = 0;
	for( int x = x_from; x < x_to; x++ ) // scan only the inner 1/3rd of the image
	{
		ConsecutiveCounter = 0;
		i = (Width*(Height/3)) + x;

		for(int y = y_from; y < y_to; y++) // scan only the inner 1/3rd of the image
        {
			if( Image(i) == color )
				ConsecutiveCounter++;
			else {
				if( ConsecutiveCounter != 0 ) {
				
					// check to verify that we just passed a full grid particle
					if( ConsecutiveCounter == grid_size_y )
					{
						if( !Passed1 ) { // just passing a full grid particle for the first time
							Passed1 = true; //mark it so we can count the spcaes
							SpaceConsecutiveCounter++;
						}
						else { // just passing a full grid particle for the second time
							grid_spacing_y = SpaceConsecutiveCounter;
							break;
						}							
					}
					else { // as long as we did not just pass a full grid particle
						Passed1 = false;
						SpaceConsecutiveCounter = 0;
					}
					
					ConsecutiveCounter = 0;
				}
				else {
					if( Passed1 )
						SpaceConsecutiveCounter++;
				}
			}
			i += Width;
		}
		
		if(grid_spacing_y)
			break;
	}

    grid_spacing_x_mm = float(grid_spacing_x) * X_MMPerPixel;
    grid_spacing_y_mm = float(grid_spacing_y) * Y_MMPerPixel;

	fprintf(out, "<Grid SpacingX = \"%d\" SpacingY = \"%d\" SpacingX_mm = \"%.03f\" SpacingY_mm = \"%.03f\" ExpectedSpacingX = \"%d\" ExpectedSpacingY = \"%d\"/>\n", grid_spacing_x, grid_spacing_y, grid_spacing_x_mm, grid_spacing_y_mm, expected_grid_spacing_x, expected_grid_spacing_y);


    // now compare the expected results and determine the pass result

	if( grid_size_x 		!= 	expected_grid_size_x 		||
		grid_size_y 		!= 	expected_grid_size_y 		||
		grid_spacing_x 		!= 	expected_grid_spacing_x 	||
		grid_spacing_y 		!= 	expected_grid_spacing_y )

		PassedAllTests = false;

	if(PassedAllTests)
    	fprintf(out, "<Result pass = \"True\" />\n");
    else
    	fprintf(out, "<Result pass = \"False\" />\n");
	fprintf(out, "</HollowGrid>\n\n");
		
	return PassedAllTests;
}

bool RunHollowShieldTests(char* ini_file_name)
{
	int ConsecutiveCounter = 0;
	bool PassedAllTests = true;
	
	int shield_size_x_left = 0;
	int shield_size_x_right = 0;
	int shield_size_y_top = 0;
	int shield_size_y_bottom = 0;

	float shield_size_x_left_mm = 0;
	float shield_size_x_right_mm = 0;
	float shield_size_y_top_mm = 0;
	float shield_size_y_bottom_mm = 0;

	int expected_shield_size_x = 0;
	int expected_shield_size_y = 0;
	
	int left_third = Width/3;
	int right_third = (Width*2)/3;
	int bottom_third = Height/3;
	int top_third = (Height*2)/3;
	
	__int64 i = 0;
	
	char ini[100] = ".\\";
	strcat(ini, ini_file_name);
	strcat(ini, ".ini");

	int color = GetPrivateProfileInt("HollowShield", "Color", INVALID_VALUE, ini);

	if(color == INVALID_VALUE) // this will indicate that we do not have a "Grid" section
	{
		return true;
	}

    expected_shield_size_x = GetPrivateProfileInt("HollowShield", "ShieldSizeX", INVALID_VALUE, ini);
    expected_shield_size_y = GetPrivateProfileInt("HollowShield", "ShieldSizeY", INVALID_VALUE, ini);

    fprintf(out, "<HollowShield>\n", i);

	// now scan to measure the spaces between the grid particles
	
	// check Horizontally - from left
	
	i = (Height/2) * Width;  // horizontal scan line is in the middle of the image
	ConsecutiveCounter = 0;
	for(int x = 0; x < left_third; x++) // scan only the inner 1/3rd of the image
	{
		if( Image(i) == color )
			ConsecutiveCounter++;
		else {
			if( ConsecutiveCounter != 0 ) {
				// we just passed the shield
				shield_size_x_left = ConsecutiveCounter;
				break;
			}
		}
		
		i++;
	}
	
	// check Horizontally - from right
	i = (Height/2) * Width;  // horizontal scan line is in the middle of the image
	ConsecutiveCounter = 0;
	for(int x = Width-1; x > right_third; x--) // scan only the inner 1/3rd of the image
	{
		if( Image(i) == color )
			ConsecutiveCounter++;
		else {
			if( ConsecutiveCounter != 0 ) {
				// we just passed the shield
				shield_size_x_right = ConsecutiveCounter;
				break;
			}
		}
		
		i--;
	}
		
	// check Vertically - from bottom
	ConsecutiveCounter = 0;
	i = Width/2;
	for(int y = 0; y < top_third; y++) // no need to scan beyond 1/3rd of the image
	{
		if( Image(i) == color )
			ConsecutiveCounter++;
		else {
			if( ConsecutiveCounter != 0 ) {
				// we just passed the shield
				shield_size_y_bottom = ConsecutiveCounter;
                break;
			}
		}
		
		i += Width;
	}
	
	// check Vertically - from top
	ConsecutiveCounter = 0;
	i = (Height * Width) - (Width/2);
	for(int y = Height-1; y > bottom_third; y--) // no need to scan beyond 1/3rd of the image
	{
		if( Image(i) == color )
			ConsecutiveCounter++;
		else {
			if( ConsecutiveCounter != 0 ) {
				// we just passed the shield
				shield_size_y_top = ConsecutiveCounter;
                break;
			}
		}
		
		i -= Width;
	}
	
    shield_size_x_left_mm = float(shield_size_x_left) * X_MMPerPixel;
	shield_size_x_right_mm = float(shield_size_x_right) * X_MMPerPixel;
    shield_size_y_bottom_mm = float(shield_size_y_bottom) * Y_MMPerPixel;
	shield_size_y_top_mm = float(shield_size_y_top) * Y_MMPerPixel;

	// the top and bottom are upside-down in Y axis, 
	// so the prints to the XML will be flipped as well:
	fprintf(out, "<Pixels Left = \"%d\" Right = \"%d\" Bottom = \"%d\" Top = \"%d\" />\n", shield_size_x_left, shield_size_x_right, shield_size_y_bottom, shield_size_y_top);
	fprintf(out, "<Length Left_mm = \"%.03f\" Right_mm = \"%.03f\" Bottom_mm = \"%.03f\" Top_mm = \"%.03f\" />\n", shield_size_x_left_mm, shield_size_x_right_mm, shield_size_y_bottom_mm, shield_size_y_top_mm );
	fprintf(out, "<ExpectedPixels SizeX = \"%d\" SizeY = \"%d\" />\n", expected_shield_size_x, expected_shield_size_y);


    // now compare the expected results and determine the pass result

    if( shield_size_x_left < expected_shield_size_x - AllowedPixelDeviation ||
        shield_size_x_left > expected_shield_size_x + AllowedPixelDeviation ||

        shield_size_x_right < expected_shield_size_x - AllowedPixelDeviation ||
        shield_size_x_right > expected_shield_size_x + AllowedPixelDeviation ||

        shield_size_y_bottom < expected_shield_size_y - AllowedPixelDeviation ||
        shield_size_y_bottom > expected_shield_size_y + AllowedPixelDeviation ||

        shield_size_y_top < expected_shield_size_y - AllowedPixelDeviation ||
        shield_size_y_top > expected_shield_size_y + AllowedPixelDeviation  )
    {

		PassedAllTests = false;

    }

	if(PassedAllTests)
    	fprintf(out, "<Result pass = \"True\" />\n");
    else
    	fprintf(out, "<Result pass = \"False\" />\n");
	fprintf(out, "</HollowShield>\n\n");
	
	return PassedAllTests;
}

bool RunRectangularTests(char* ini_file_name)
{
	//int x = 0;
	int y_from = 0;
	int y_to = 0;
	//int y = 0;
	int x_from = 0;
	int x_to = 0;
	int expected = 0;
	int color = 0;
	int alt_color = 0;
	int pixel_counter = 0;

	//float mm = 0;
	//float mm_expected = 0;

	int i = 1;
    int line_count = 0;

	bool PassedAllTests = true;

	char ini[100] = ".\\";
	strcat(ini, ini_file_name);
	strcat(ini, ".ini");

	char rect[10] = "rectangle";
	char rect_i[10] = {0};
	char str_i[10] = {0};

	while(true) // for line_? tests
	{
		itoa(i, str_i, 10);
		rect_i[0] = NULL;
		strcat(rect_i, rect);
		strcat(rect_i, str_i);

		if( (expected = GetPrivateProfileInt(rect_i, "expected", INVALID_VALUE, ini)) == INVALID_VALUE )
          break;

		if( (color = GetPrivateProfileInt(rect_i, "color", INVALID_VALUE, ini)) == INVALID_VALUE )
          break;

		alt_color = GetPrivateProfileInt(rect_i, "alt_color", INVALID_VALUE, ini);

        //h_line_count_threshold = GetPrivateProfileInt(rect_i, "h_line_count_threshold", INVALID_VALUE, ini);

		if( (x_from = GetPrivateProfileInt(rect_i, "x_from", INVALID_VALUE, ini)) == INVALID_VALUE )
          break;

		if( (x_to = GetPrivateProfileInt(rect_i, "x_to", INVALID_VALUE, ini)) == INVALID_VALUE )
          break;

        if( (y_from = GetPrivateProfileInt(rect_i, "y_from", INVALID_VALUE, ini)) == INVALID_VALUE )
          break;

        y_from = Height - 1 - y_from;

        if( (y_to = GetPrivateProfileInt(rect_i, "y_to", INVALID_VALUE, ini)) == INVALID_VALUE )
          break;

        y_to = Height - 1 - y_to;

        line_count = 0;
        pixel_counter = 0;

		for( int y = y_from; y >= y_to; y-- )  // we run downwards because the y values are upside-down
		{
            line_count = HTest(y, x_from, x_to, color, alt_color);

            // i assume that if there is a straight line of the desired color, it goes all through the rectangle.
            // so if the count was smaller than the width of the rectangle, i add it to the total count
            // (else - this is a line that i want to ignore so i don't add it)
            if( line_count < x_to - x_from + 1 )
                pixel_counter += line_count;

//			mm = float(pixel_counter) * Y_MMPerPixel;
//			mm_expected = float(expected) * Y_MMPerPixel;
        }

        fprintf(out, "<Rectangle num=\"%d\">\n", i);
        fprintf(out, "<Coordinates x_From = \"%ld\" x_To = \"%ld\" yFrom = \"%ld\" yTo = \"%ld\" />\n", x_from, x_to, (Height - 1 - y_from), (Height - 1 - y_to) ); //only for displaying the results, convert the height back to user coordinates (instead of image pixel coordinates)
        fprintf(out, "<Scan Color = \"%s\" AlternativeColor = \"%s\" />\n", ColorCode2Str(color), ColorCode2Str(alt_color) );
        fprintf(out, "<Count pixels = \"%ld\" expected = \"%ld\" />\n", pixel_counter, expected);
        //fprintf(out, "<Length_Y mm = \"%.03f\" expected = \"%.03f\" />\n", mm, mm_expected);

        //if( pixel_counter != expected )
        if( pixel_counter < expected - AllowedPixelDeviation ||
            pixel_counter > expected + AllowedPixelDeviation )
        {
            fprintf(out, "<Result pass = \"False\" />\n");
            PassedAllTests = false;
        }
        else
        {
            fprintf(out, "<Result pass = \"True\" />\n");
        }

        fprintf(out, "</Rectangle>\n\n");

        i++;
	}

	return PassedAllTests;
}

bool RunTests(char* ini_file_name)
{
	bool PassedAllTests = true;

	fprintf(out, "<Test>\n");

    fprintf(out, "<Machine Type = \"%d\" />\n", MachineType);

	PassedAllTests = ( 	RunLineTests(ini_file_name) &&
						RunAdherenceTests(ini_file_name) &&
						RunGridTests(ini_file_name) &&
                        RunHollowGridTests(ini_file_name) &&
                        RunHollowShieldTests(ini_file_name) &&
                        RunRectangularTests(ini_file_name) );
	
	if( PassedAllTests ) {
		fprintf(out, "<Result Pass = \"True\" />\n");
	}
	else {
		fprintf(out, "<Result Pass = \"False\" />\n");
	}

	fprintf(out, "</Test>\n");

	return PassedAllTests;
}

int main(int argc, char* argv[])
{
	 bool fail = false;
	 bool init_fail = false;

	 char xml[100] = ".\\";
	 strcat(xml, ( argc == 2 ) ? argv[1] : FILE_NAME );
	 strcat(xml, ".xml");

	 out = fopen( xml, "wt");

	 if( out )
	 {

		 //fprintf(out, "<?xml version=\"1.0\"?>\n");

		 // EXE BMP
		 if( ! ReadImage( (argc == 2)? argv[1] : FILE_NAME ) ) {
				 fail = true;
		 }
		 
		 if( ! fail )
		 {
			if( ! Init() )
				init_fail = true;
		 }

		 if( ! fail && ! init_fail )
		 {
			 RunTests( (argc == 2) ? argv[1] : FILE_NAME );
		 }
		 else {
			if(fail)
				fprintf(out, "<Error desc = \"Fail to load image\" />\n");
			else // init_fail
				fprintf(out, "<Error desc = \"Invalid params in defines.ini\" />\n");
		 }

		 fclose(out);
		 
	 }
	 else {

		 printf("\nCannot write XML output file! Aborting... \n\n");

	 }

	 //printf("\nPress ENTER to exit");
	 //getc(stdin);


	 return 0;
}
//---------------------------------------------------------------------------