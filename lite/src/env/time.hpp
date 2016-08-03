
class Time
{
public:
    static core::String now()
    {
        time_t rawtime;
        ::time(&rawtime);
        char buffer[0x4F];
        struct tm* timeinfo = ::localtime(&rawtime);
        ::strftime(buffer, sizeof(buffer), "%Z.%y-%m-%d.%H:%M:%S", timeinfo);
        return ascii_nlen(buffer);
    }
public:
    Time()
    {
        the_start = ::clock();
    }
    core::uint32 mark()
    {
        ::clock_t stop = ::clock();
        ::clock_t result = stop - the_start;
        the_start = stop;
        return 1000 * result / CLOCKS_PER_SEC; // miliseconds
    }
private:
    clock_t the_start;
};
