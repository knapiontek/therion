
class CLang : FormatWriter
{
	public string finalDef =
		@"@typedef char int1_t;
		typedef short int2_t;
		typedef int int4_t;
		typedef long int8_t;
		typedef unsigned char uint1_t;
		typedef unsigned short uint2_t;
		typedef unsigned int uint4_t;
		typedef unsigned long uint8_t;
		typedef double float8_t;
		typedef long double float12_t;";
	public string typeDef =
		@"@struct %
		{%
		};
		typedef struct % %_t;";
	public string copyDef =
		@"@void %_copy(%_t* p1, %_t* p2)
		{%
		}";
	public string callDef =
		@"@void %_call(%%)
		{%
		}";
	public string mainDef =
		@"@int main(int argc, char* argv[])
		{
			%_call();
			return 0;
		}";
	public string thatDef =
		@"@struct %* %;";
	public string fieldDef =
		@"@%_t %;";
	public string firstParamDef =
		@"%_t* %";
	public string finalNextParamDef =
		@", %_t %";
	public string typeNextParamDef =
		@", %_t* %";
	public string finalCopy =
		@"@p1->% = p2->%;";
	public string typeCopy =
		@"@%_copy(&p1->%, &p2->%);";
	public string thatCall =
		@"@%->% = %;";
	public string typeWithThatCall =
		@"@%_call(%, %);";
	public string typeCall =
		@"@%_call(%);";
	public string typeCopyCall =
		@"@%_copy(%, %);";
	public string typeLocalCopyCall =
		@"@%_t %;
		%_copy(&%, %);";
	public string paramCall =
		@", %";
	public string finalLocalCall =
		@"@%_t % = %;";
	public string opLocalCall =
		@"@%_t % = % % %;";
	public string typeLocalCall =
		@"@%_t %;
		%_call(&%%);";
	public string finalTailCall =
		@"@% = %;";
	public string opTailCall =
		@"@% = % % %;";
	public string typeTailCall =
		@"@%_call(%%);";
	public string label =
		@"@%:";
	public string goToLabel =
		@"@goto %;";
	public string caseBlock =
		@"@if(%)
		{%
		}";
}
