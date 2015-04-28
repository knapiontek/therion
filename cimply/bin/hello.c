
typedef char int1_t;
typedef short int2_t;
typedef int int4_t;
typedef long int8_t;
typedef unsigned char uint1_t;
typedef unsigned short uint2_t;
typedef unsigned int uint4_t;
typedef unsigned long uint8_t;
typedef double float8_t;
typedef long double float12_t;
struct hello
{
	int4_t i3;
};
typedef struct hello hello_t;
hello_t hello;
void hello_call()
{
	loop2:
	hello.i3 = -1;
	none1_t none1 = hello.i3 < 0;
	if(none1)
	{
		int4_t x6 = hello.i3;
	}
	none1_t none1;
	none1_copy(&none1, &true);
	if(none1)
	{
		int4_t x8 = 2;
	}
	loop2_t none1 = hello.loop2;
}
int main(int argc, char* argv[])
{
	hello_call();
	return 0;
}