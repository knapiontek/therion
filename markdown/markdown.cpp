#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>

enum class Mode { None, Line, List, Table };

int nested_tables = 0;
std::vector<int> nested_list;

void convert(const std::string& md)
{
    Mode mode;
    char chars[3] = { '\0', '\0', '\0' };

    std::string html = md;
    int index = md.rfind('.');
    if (index != std::string::npos)
        html = md.substr(0, index - 1);
    html += ".html";

    std::ifstream in(md, std::ios::in);
    std::ofstream out(html, std::ios::out);

    while (!in.eof())
    {
        chars[2] = chars[1];
        chars[1] = chars[0];
        chars[0] = in.get();
        switch (chars[0])
        {
        case '\n':
            mode = Mode::Line;
            goto action;
        case '*':
            mode = Mode::List;
            goto action;
        case '#':
            goto action;
        case '[':
            mode = Mode::Table;
            out << "\n</table>\n<tr>";
            break;
        case '|':
            out << "\n</td>\n<td>";
            break;
        case ']':
            out << "\n</tr>\n</table>";
            break;
        default:
        action:
            if (mode == Mode::Table)
            {
            }
            out << chars[0];
        }
    }
}


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
            throw std::invalid_argument("expected: markdown <file>.md");
        else
            convert(argv[1]);
        return 0;
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
}
