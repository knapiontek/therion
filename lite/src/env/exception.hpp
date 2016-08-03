
class Exception
{
public:
    core::String& message()
    {
        return the_message;
    }
    Runtime& runtime()
    {
        return the_runtime;
    }
private:
    Runtime the_runtime;
    core::String the_message;
};
