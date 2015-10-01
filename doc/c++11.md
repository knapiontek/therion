
## auto

```c_cpp
template<class T> void printall(const vector<T>& v)
{
    for (auto p = v.begin(); p!=v.end(); ++p)
        cout << *p << "\n";
}
```

```c_cpp
template<class T, class U>
auto mul(T x, U y) -> decltype(x*y)
{
    return x*y;
}
```

## brackets

```c_cpp
std::list<std::vector<std::string>> lvs;
```

## enum class

```c_cpp
enum class Color { red, blue };
enum class Color : char { red, blue };
int a5 = Color::blue;     // error: not Color->int conversion
Color a6 = Color::blue;   // ok
```

## Initializer lists

```c_cpp
vector<double> v = { 1, 2, 3.456, 99.99 };
```

```c_cpp
template<class E> class vector {
public:
    vector (std::initializer_list<E> s) // initializer-list constructor
    {
        reserve(s.size());      // get the right amount of space
        uninitialized_copy(s.begin(), s.end(), elem);   // initialize elements (in elem[0:s.size()))
        sz = s.size();  // set vector size
    }
};
```

## shared pointer

```c_cpp
std::shared_ptr<int> foo2 (new int(10)); // 2 malloc's

auto bar = std::make_shared<int> (20); // reference counter allocated in front of the array
```

## In-class member initializers

```c_cpp
class A {
public:
    int a = 7;
};
```

## Raw string literals

```c_cpp
string s = "\\w\\\\\\w"; // I hope I got that right
string s = R"(\w\\\w)"; // I'm pretty sure I got that right
```

## Threads

```c_cpp
std::mutex m;
```

## Lambdas in asio

```c_cpp
socket.async_connect(server_endpoint,
                     [&](std::error_code ec) {
                         if (!ec) {
                             socket.async_read_some(asio::buffer(data),
                                                    [&](std::error_code ec, std::size_t length) {
                                                        if (!ec) {
                                                            async_write(socket,
                                                                        asio::buffer(data, length),
                                                                        [&](std::error_code ec, std::size_t length) {
                                                                            // ...
                                                                        });
                                                        }
                                                    });
                         }
                     });
```
