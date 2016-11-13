
#include<malloc.h>

struct TestStruct
{
  int i;
};

int main()
{
  TestStruct ts;
  ts.i = 999;
  return ts.i;
}
