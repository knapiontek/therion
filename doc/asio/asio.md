
![boost](https://github.com/knapiontek/therion/blob/master/doc/asio/boost.png)

# boost::asio in context of C++11 and event-driven programming

## Multithreading problem

### Context Switching Overhead

    - std::this_thread::yield()
    - std::mutex::lock()
    - ::read(int fd, void *buf, size_t count);

#### Only 5% overhead for single thread

     - 5% message processing
     - 5% context switching
     - 90% idle CPU - networking

#### 50% overhead for 10 simultaneous threads

     - 50% message processing
     - 50% context switching
     - 0% idle CPU

#### not HTTP server is writen in this way

## Event-Driven Programming

### Node.js example

```javascript
fs.readFile('/etc/passwd',
    function (err, data) {
        if (err)
            throw err;
        console.log(data);
    });
```

![boost](https://github.com/knapiontek/therion/blob/master/doc/asio/nodejs.svg)

## Lambda syntax in C++11

### The most trivial case

```c++
int main()
{
    auto lambda = [] () { cout << "Hello world"; };
    lambda();
}
```

```c++
int main()
{
    string hello = "Hello world";
    auto lambda = [&] () { cout << hello; };
    lambda();
}
```

## Lambdas in asio

![boost](https://github.com/knapiontek/therion/blob/master/doc/asio/asio.svg)

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
