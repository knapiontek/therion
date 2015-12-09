asio

![boost](https://github.com/knapiontek/therion/blob/master/doc/asio/boost.png)

## lambda syntax in C++11

```c++
#include <iostream>

int main()
{
    auto func = [] () { std::cout << "Hello world"; };
    func(); // now call the function
}
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
