
inline void test_lang()
{
    core::verify(env::arg_seq().size() == 2);

    auto lite_filename = core::Format("$1/sample/start.lite")
            .arg(env::arg_seq().at(1))
            .end();
    io::File lite_file(lite_filename);
    core::verify(lite_file.status(io::File::exist));
    core::verify(lite_file.try_open());

    lang::Tree tree;
    io::Decode decode = lite_file.input();
    lang::Reader::execute(tree, decode, lite_filename);
    lang::Writer::execute(tree);
}
