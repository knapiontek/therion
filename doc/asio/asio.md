asio


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
