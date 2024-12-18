
## auto

```c++
template<class T> void printall(const vector<T>& v)
{
    for (auto p = v.begin(); p!=v.end(); ++p)
        cout << *p << "\n";
}
```

```c++
template<class T, class U>
auto mul(T x, U y) -> decltype(x*y)
{
    return x*y;
}
```

## brackets

```c++
std::list<std::vector<std::string>> lvs;
```

## enum class

```c++
enum class Color { red, blue };
enum class Color : char { red, blue };
int a5 = Color::blue;     // error: not Color->int conversion
Color a6 = Color::blue;   // ok
```

## Initializer lists

```c++
vector<double> v = { 1, 2, 3.456, 99.99 };
```

```c++
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

```c++
std::shared_ptr<int> foo2 (new int(10)); // 2 malloc's

auto bar = std::make_shared<int> (20); // reference counter allocated in front of the array
```

## In-class member initializers

```c++
class A {
public:
    int a = 7;
};
```

## Raw string literals

```c++
string s = "\\w\\\\\\w"; // I hope I got that right
string s = R"(\w\\\w)"; // I'm pretty sure I got that right
```

## Threads

```c++
std::mutex m;
```

## Lambdas in asio

```c++
socket.async_connect(server_endpoint,
    [&](std::error_code ec) // lambda1
    {
        if (!ec)
        {
            socket.async_read_some(asio::buffer(data),
                [&](std::error_code ec, std::size_t length) // lambda2
                {
                    if (!ec)
                    {
                        async_write(socket,
                            asio::buffer(data, length),
                            [&](std::error_code ec, std::size_t length) // lambda3
                            {
                                // ...
                            });
                    }
                });
        }
    });
```
