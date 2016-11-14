
#include<malloc.h>

struct TestStruct
{
  int i;
};

void destroy_struct(TestStruct* pts)
{
  free(pts);
}

int main()
{
  TestStruct ts;
  TestStruct* pts = &ts;
  destroy_struct(pts);
  pts->i = 999;
  return ts.i;
}
