
![boost](boost.png)


# boost::asio in context of C++11 and event-driven programming


###### Asynchronous I/O




## Multithreading problem


##### Context Switching Overhead

    - std::this_thread::yield()
    - std::mutex::lock()
    - ::read(int fd, void *buf, size_t count);

##### Only 5% overhead for single thread

     - 5% message processing
     - 5% context switching
     - 90% idle CPU - networking

##### 50% overhead for 10 simultaneous threads

     - 50% message processing
     - 50% context switching
     - 0% idle CPU

###### no HTTP server is writen in this way




## Event-Driven Programming


##### Node.js example

```javascript
fs.readFile('/etc/passwd',
    function (err, data) {
        if (err)
            throw err;
        console.log(data);
    });
```

![Node.js](nodejs.png)




##### poll() function

```c++
int poll(struct pollfd fds[], nfds_t nfds, int timeout); 
```

##### example

```c++
#include <stropts.h>
#include <poll.h>
...
struct pollfd fds[2];
int timeout_msecs = 500;
int ret;
int i;

/* Open STREAMS device. */
fds[0].fd = open("/dev/dev0", ...);
fds[1].fd = open("/dev/dev1", ...);
fds[0].events = POLLOUT | POLLWRBAND;
fds[1].events = POLLOUT | POLLWRBAND;

ret = poll(fds, 2, timeout_msecs);

if (ret > 0) {
    for (i=0; i<2; i++) {
        if (fds[i].revents & POLLWRBAND) {
            ...
        }
        if (fds[i].revents & POLLOUT) {
            ...
        }
        if (fds[i].revents & POLLHUP) {
            ...
        }
    }
}
```




## Lambda syntax in C++11

##### The most trivial case

```c++
int main()
{
    auto lambda = [] () { cout << "Hello world"; };
    lambda();
}
```

##### variable capture in lambda

```c++
int main()
{
    string hello = "Hello";
    auto lambda = [&] (const string& world) { cout << hello << world; };
    lambda(" world");
}
```




## Lambdas in boost::asio

![ASIO](asio.png)

```c++
socket.async_connect(server_endpoint,
    [&](std::error_code ec) // lambda1
    {
        socket.async_read_some(asio::buffer(data),
            [&](std::error_code ec, std::size_t length) // lambda2
            {
                    async_write(socket,
                        asio::buffer(data, length),
                        [&](std::error_code ec, std::size_t length) // lambda3
                        {
                            // ...
                        });
            });
    });
```




### Issues related to event-driven programming

    - thread stack very small
    - heap usually used instead of stack
    - debugging
    - code flow may not be clear
    - trailing references