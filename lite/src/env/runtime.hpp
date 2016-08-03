
class Runtime
{
    struct Handle;
public:
    struct Frame
    {
        const char* function_name;
        const char* file_name;
        unsigned int line;
    };
    class Iterator
    {
    public:
        Iterator(const Runtime& runtime)
        {
            the_handle = runtime.the_handle;
            the_init = false;
            the_found = false;
            the_demangled = 0;
            the_index = 0;
            the_bfd = 0;
            the_syms = 0;

            bfd_init();

            char file_name[core::uint8_max];
            ::snprintf(file_name, sizeof(file_name), "/proc/%d/exe", getpid());

            the_bfd = bfd_openr(file_name, 0);
            if(!the_bfd)
                return;

            if(bfd_check_format(the_bfd, bfd_archive))
                return;

            char** matching;
            if(!bfd_check_format_matches(the_bfd, bfd_object, &matching))
            {
                ::free(matching);
                return;
            }

            if((!bfd_get_file_flags(the_bfd) & HAS_SYMS))
                return;

            bfd_boolean dynamic = false;
            long storage = bfd_get_symtab_upper_bound(the_bfd);
            if(storage)
            {
                storage = bfd_get_dynamic_symtab_upper_bound(the_bfd);
                dynamic = true;
            }
            if(0 > storage)
                return;

            the_syms = (asymbol**)malloc(storage);
            long sym_cnt = dynamic
                ? bfd_canonicalize_dynamic_symtab(the_bfd, the_syms)
                : bfd_canonicalize_symtab(the_bfd, the_syms);
            if(0 > sym_cnt)
                return;

            the_init = true;
        }
        ~Iterator()
        {
            if(the_demangled)
                ::free(the_demangled);
            if(the_syms)
                ::free(the_syms);
            if(the_bfd)
                bfd_close(the_bfd);
        }
        bool next()
        {
            if(the_found)
            {
                the_found = bfd_find_inliner_info(the_bfd, &the_frame.file_name, &the_frame.function_name, &the_frame.line);
                if(the_found)
                    return true;
            }
            while(the_init && the_index < the_handle->size)
            {
                the_pc = (bfd_vma)the_handle->path[the_index++];
                bfd_map_over_sections(the_bfd, find_address_in_section, this);
                if(!the_found || !the_frame.function_name || !the_frame.file_name)
                    continue;
                const char* slash = ::strrchr(the_frame.file_name, '/');
                if(slash)
                    the_frame.file_name = slash + 1;
                if(the_demangled)
                    ::free(the_demangled);
                the_demangled = bfd_demangle(the_bfd, the_frame.function_name, DMGL_ANSI | DMGL_PARAMS);
                if(the_demangled)
                    the_frame.function_name = the_demangled;
                return true;
            }
            return false;
        }
        Frame& value()
        {
            return the_frame;
        }
    private:
        static void find_address_in_section(bfd *abfd, asection *section, void *cookie)
        {
            auto it = (Iterator*)cookie;

            bfd_vma vma;
            bfd_size_type size;

            if(it->the_found)
                return;

            if(!(bfd_get_section_flags(abfd, section) & SEC_ALLOC))
                return;

            vma = bfd_get_section_vma(abfd, section);
            if(it->the_pc < vma)
                return;

            size = bfd_get_section_size(section);
            if(it->the_pc >= vma + size)
                return;

            it->the_found = bfd_find_nearest_line(abfd, section, it->the_syms, it->the_pc - vma,
                &it->the_frame.file_name, &it->the_frame.function_name, &it->the_frame.line);
        }
    private:
        bfd* the_bfd;
        asymbol** the_syms;
        bfd_boolean the_found;
        bfd_vma the_pc;
        char* the_demangled;
        bool the_init;
        core::uint32 the_index;
        Frame the_frame;
        Handle* the_handle;
    };
public:
    static Runtime backtrace()
    {
        return Runtime();
    }
public:
    Runtime()
    {
        // Runtime is used in an error handling of ::malloc(), therefore cannot use ::malloc()!
        const core::uint32 size = 64;
        static __thread core::uint8 space[sizeof(Handle) + sizeof(void*) * size];
        the_handle = (Handle*)space;
        the_handle->size = ::backtrace(the_handle->path, size);
    }
private:
    struct Handle
    {
        core::uint32 size;
        void* path[1];
    };
private:
    Handle* the_handle;
};
