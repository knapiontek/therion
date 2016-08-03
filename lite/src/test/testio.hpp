
core::String cs_filetest_tmp = cs("/tmp/filetest.tmp");
core::String cs_tmp = cs(".tmp");
core::String cs_file_format = cs("$1/$2.$3");
core::String cs_slash = cs("/");
core::String cs_host = cs("::1");
core::String cs_outgoing = cs("the_outgoing");
core::String cs_incoming = cs("the_incoming");
core::String cs_www_google_com = cs("www.google.com");
core::String cs_http = cs("http");

const core::uint16 the_port = 8071;

inline void test_file()
{
    core::String message = cs_host;
    io::File file(cs_filetest_tmp);

    // assemble filename
    core::String filename = core::Format(cs_file_format)
        .arg(file.path())
        .arg(file.core())
        .arg(file.ext())
        .end();
    core::verify(filename.equal(cs_filetest_tmp));
    filename = file.path()
        .append(cs_slash)
        .append(file.local());
    core::verify(filename.equal(cs_filetest_tmp));

    // write
    file.name().append(cs_tmp);
    file.open(io::File::read_write);
    io::Encode encode = file.output();
    encode.write_st(cs_outgoing);
    file.close();

    // exist
    core::verify(file.status(io::File::exist));

    // read
    file.open(io::File::read_only);
    io::Decode decode = file.input();
    decode.read_st(message);
    core::verify(message.equal(cs_outgoing));
    file.close();

    // status
    core::verify(file.status(io::File::exist | io::File::readable | io::File::writable | io::File::executable));

    // rename, remove
    core::verify(file.rename(core::String(file.name()).append(cs_tmp)));
    core::verify(file.remove());
}

inline void test_io()
{
    test_file();
}
