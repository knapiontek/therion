
class Time
{
public:
    static core::String now()
    {
        time_t rawtime;
        ::time(&rawtime);
        char buffer[0x4F];
        auto timeinfo = ::localtime(&rawtime);
        ::strftime(buffer, sizeof(buffer), "%Z.%y-%m-%d.%H:%M:%S", timeinfo);
        return buffer;
    }
public:
    Time()
    {
        the_start = ::clock();
    }
    core::int64 mark()
    {
        auto stop = ::clock();
        auto result = stop - the_start;
        the_start = stop;
        return 1000 * result / CLOCKS_PER_SEC; // miliseconds
    }
private:
    clock_t the_start;
};
