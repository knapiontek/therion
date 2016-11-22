
inline void test_lang()
{
    core::verify(2 == env::arg_seq().size());

    auto lite_filename = core::Format("%1/sample/start.lite")
        % env::arg_seq().at(1)
        % core::end;
    io::File lite_file(lite_filename);
    core::verify(lite_file.status(io::File::exist));
    core::verify(lite_file.try_open());

    lang::Tree tree;
    lang::Decode decode = lite_file.input();
    lang::Reader::execute(tree, decode, lite_filename);
    lang::Writer::execute(tree, lite_filename);
}
