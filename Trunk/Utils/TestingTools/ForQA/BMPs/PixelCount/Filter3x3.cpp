/*
char edge_1[FILTER_SIZE][FILTER_SIZE] = {
  YELLOW, BLACK, BLACK,
  YELLOW, BLACK, BLACK,
  YELLOW, BLACK, BLACK };

char edge_2[FILTER_SIZE][FILTER_SIZE] = {
  BLACK, BLACK, YELLOW,
  BLACK, BLACK, YELLOW,
  BLACK, BLACK, YELLOW };

char edge_3[FILTER_SIZE][FILTER_SIZE] = {
  YELLOW, YELLOW, YELLOW,
  BLACK,  BLACK,  BLACK,
  BLACK,  BLACK,  BLACK };

char edge_4[FILTER_SIZE][FILTER_SIZE] = {
  BLACK,  BLACK,  BLACK,
  BLACK,  BLACK,  BLACK,
  YELLOW, YELLOW, YELLOW};
*/


/*
bool MatchFilter(char arr[][FILTER_SIZE], long i, long j)
{
  for(int y = FILTER_MIDDLE_INDEX * -1; y <= FILTER_MIDDLE_INDEX; y++) {
    for(int x = FILTER_MIDDLE_INDEX * -1; x <= FILTER_MIDDLE_INDEX; x++) {
      if( image[i+y][j+x] != arr[FILTER_MIDDLE_INDEX+y][FILTER_MIDDLE_INDEX+x] )
        return false;
    }
  }
  return true;
}

bool IsModelEdge(long i, long j)
{
  if( MatchFilter(edge_1, i, j) )
    return true;

  if( MatchFilter(edge_2, i, j) )
    return true;

  if( MatchFilter(edge_3, i, j) )
    return true;

  if( MatchFilter(edge_4, i, j) )
    return true;

  return false;
}
*/

/*
     while ( !feof(fp) )
     {
        ch = fgetc(fp);

        if( ch != -1 ) {
          colors[ch]++;

          //put the pixels in the image array
          image[i++] = ch;

          //j++; if(j==WIDTH) { j=0; i++; }
        }
     }
*/