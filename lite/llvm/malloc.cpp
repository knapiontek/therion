
#include<malloc.h>

struct TestStruct
{
  int i;
  double d;
};

int main()
{
  TestStruct* ts = (TestStruct*)malloc(sizeof(TestStruct));
  ts->i = 1;
  ts->d = 2.0;
  return int(long(ts));
}
