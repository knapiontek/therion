
struct core_employee_type
{
	int result;
};

void core_employee_call(struct core_employee_type* core_employee, void (*compare)(int* ret, int x))
{
	compare(&core_employee->result, 6);
}

void core_my_compare_call(int* ret, int x)
{
	*ret = x + 7;
}

int main()
{
	struct core_employee_type core_myself;
	core_employee_call(&core_myself, core_my_compare_call);
	return core_myself.result;
}
