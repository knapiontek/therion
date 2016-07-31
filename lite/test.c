
#include <stdio.h>

int inner()
{
  return 9;
}

int main()
{
  int x = 5;
  if (x < 4) x = 6;
  printf("\nx: %d\n", x);
  x = inner();
  return 1;
}
