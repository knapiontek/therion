
class File
{
public:
    enum Mode
    {
        read_only = O_RDONLY,
        write_only = O_CREAT | O_WRONLY,
        read_write = O_CREAT | O_RDWR
    };
    enum
    {
        exist = F_OK,
        readable = R_OK,
        writable = W_OK,
        executable = X_OK
    };
public:
    static bool make_folder(core::String& name)
    {
        return !::mkdir(name.ascii(), S_IRWXU);
    }
    static bool make_folders(core::String& name)
    {
        for(auto it : name)
        {
            if('/' == it.value())
            {
                core::String part(name.data(), it.position());
                if(::mkdir(part.ascii(), S_IRWXU) && EEXIST != errno)
                    return false;
            }
        }
        return !::mkdir(name.ascii(), S_IRWXU);
    }
    static bool status(core::String& name, core::uint32 status)
    {
        return !::access(name.ascii(), status);
    }
    static bool rename(core::String& new_name, core::String& old_name)
    {
        return !::rename(old_name.ascii(), new_name.ascii());
    }
    static bool remove(core::String& name)
    {
        return !::remove(name.ascii());
    }
    static core::String& slash()
    {
        static core::String slash = "/";
        return slash;
    }
public:
    File()
    {
        the_handle = 0;
        the_input.the_file = this;
        the_output.the_file = this;
        the_remove_on_close = false;
    }
    File(core::String& name)
    {
        the_handle = -1;
        the_input.the_file = this;
        the_output.the_file = this;
        the_name = name;
        the_remove_on_close = false;
    }
    ~File()
    {
        if(-1 != the_handle)
        {
            ::close(the_handle);
            the_handle = -1;
            if(the_remove_on_close)
                ::remove(the_name.ascii());
        }
    }
    bool is_open()
    {
        return the_handle;
    }
    bool try_open(Mode mode = read_only, bool remove_on_close = false)
    {
        core::assert(-1 == the_handle);
        the_remove_on_close = remove_on_close;
        the_handle = ::open(the_name.ascii(), mode, S_IRWXU);
        return (-1 != the_handle);
    }
    void open(Mode mode = read_only, bool remove_on_close = false)
    {
        if(!try_open(mode, remove_on_close))
            env::Fail("open file fail: $1").arg(the_name.ascii()).fire();
    }
    void close()
    {
        if(-1 != the_handle)
        {
            ::close(the_handle);
            the_handle = -1;
            the_remove_on_close = false;
        }
    }
    core::uint32 size()
    {
        struct stat info;
        if(::stat(the_name.ascii(), &info))
            env::Fail("read file size fail: $1").arg(the_name.ascii()).fire();
        return info.st_size;
    }
    core::String& name()
    {
        return the_name;
    }
    core::String path()
    {
        core::String::Iterator it = the_name.tail();
        while(it.prev())
        {
            if('/' == it.value())
                return core::String(the_name.data(), it.position());
        }
        return the_name;
    }
    core::String local()
    {
        for(auto it : core::reverse(the_name))
        {
            if('/' == it.value())
            {
                core::uint32 position = it.position() + 1;
                return core::String(the_name.data() + position, the_name.size() - position);
            }
        }
        return core::nil;
    }
    core::String core()
    {
        core::uint32 dot = the_name.size();
        for(auto it : core::reverse(the_name))
        {
            if('.' == it.value())
                dot = it.position();
            else if('/' == it.value())
            {
                core::uint32 position = it.position() + 1;
                return core::String(the_name.data() + position, dot - position);
            }
        }
        return the_name;
    }
    core::String ext()
    {
        for(auto it : core::reverse(the_name))
        {
            if('.' == it.value())
            {
                core::uint32 position = it.position() + 1;
                return core::String(the_name.data() + position, the_name.size() - position);
            }
        }
        return core::nil;
    }
    bool make_folder()
    {
        return !::mkdir(the_name.ascii(), S_IRWXU);
    }
    bool make_folders()
    {
        for(auto it : the_name)
        {
            if('/' == it.value())
            {
                core::String part(the_name.data(), it.position());
                if(::mkdir(part.ascii(), S_IRWXU) && EEXIST != errno)
                    return false;
            }
        }
        return !::mkdir(the_name.ascii(), S_IRWXU);
    }
    bool status(core::uint32 status)
    {
        return !::access(the_name.ascii(), status);
    }
    bool rename(core::String& name)
    {
        bool result = !::rename(the_name.ascii(), name.ascii());
        if(result)
            the_name = name;
        return result;
    }
    bool remove()
    {
        return !::remove(the_name.ascii());
    }
    Input& input()
    {
        return the_input;
    }
    Output& output()
    {
        return the_output;
    }
private:
    class FileInput : Input
    {
        friend class File;
        void input(core::uint8* data, core::uint32 size)
        {
            core::uint32 read = ::read(the_file->the_handle, data, size);
            if(read != size)
                env::Fail("read data fail: block size: $1").arg(size).fire();
        }
        File* the_file;
    };
    class FileOutput : Output
    {
        friend class File;
        void output(core::uint8* data, core::uint32 size)
        {
            core::uint32 written = ::write(the_file->the_handle, data, size);
            if(written != size)
                env::Fail("write data fail: block size: $1").arg(size).fire();
        }
        File* the_file;
    };
private:
    core::int32 the_handle;
    FileInput the_input;
    FileOutput the_output;
    core::String the_name;
    bool the_remove_on_close;
};
