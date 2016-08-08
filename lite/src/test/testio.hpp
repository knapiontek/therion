
inline void test_file()
{
    core::String cs_tmp = ".tmp";
    core::String cs_filetest_tmp = "/tmp/filetest.tmp";
    io::File file(cs_filetest_tmp);

    // assemble filename
    auto filename = core::Format("$1/$2.$3")
        .arg(file.path())
        .arg(file.core())
        .arg(file.ext())
        .end();
    core::verify(filename.equal(cs_filetest_tmp));
    filename = file.path()
        .append(io::File::slash())
        .append(file.local());
    core::verify(filename.equal(cs_filetest_tmp));

    // write
    file.name().append(cs_tmp);
    file.open(io::File::read_write);
    io::Encode encode = file.output();
    core::String cs_outgoing = "the_outgoing";
    encode.write_st(cs_outgoing);
    file.close();

    // exist
    core::verify(file.status(io::File::exist));

    // read
    core::String message = "::1";
    file.open(io::File::read_only);
    io::Decode decode = file.input();
    decode.read_st(message);
    core::verify(message.equal(cs_outgoing));
    file.close();

    // status
    core::verify(file.status(io::File::exist | io::File::readable | io::File::writable | io::File::executable));

    // rename, remove
    core::verify(file.rename(file.name().clone().append(cs_tmp)));
    core::verify(file.remove());
}

inline void test_io()
{
    test_file();
}
