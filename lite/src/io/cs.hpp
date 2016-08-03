
core::String cs_open_file_fail =
    cs_eng("open file fail: $1")
    cs_pl("błąd otwarcia pliku: $1");

core::String cs_read_file_size_fail =
    cs_eng("read file size fail: $1")
    cs_pl("błąd odczytu rozmiaru pliku: $1");

core::String cs_read_data_fail =
    cs_eng("read data fail: block size: $1")
    cs_pl("błąd odczytu danych: rozmiar bloku: $1");

core::String cs_write_data_fail =
    cs_eng("write data fail: block size: $1")
    cs_pl("błąd zapisu danych: rozmiar bloku: $1");

core::String cs_receive_data_fail =
    cs_eng("receive data fail: block size: $1")
    cs_pl("błąd odebrania danych: rozmiar bloku: $1");

core::String cs_send_data_fail =
    cs_eng("send data fail: block size: $1")
    cs_pl("błąd wyslania danych: rozmiar bloku: $1");

core::String cs_connect_fail =
    cs_eng("connect fail address: [$1]:$2")
    cs_pl("błąd połączenia z adresem: [$1]:$2");

core::String cs_connect_address_fail =
    cs_eng("connect fail: invalid address")
    cs_pl("błąd połączenia: niepoprawny adress");

core::String cs_bind_address_fail =
    cs_eng("bind address fail: [$1]:$2")
    cs_pl("błąd (bind) adres: [$1]:$2");

core::String cs_accept_fail =
    cs_eng("::accept fail")
    cs_pl("błąd ::accept");

core::String cs_epoll_create1_fail =
    cs_eng("::epoll_create1 fail")
    cs_pl("błąd ::epoll_create1");

core::String cs_eventfd_fail =
    cs_eng("::eventfd fail")
    cs_pl("błąd ::eventfd");

core::String cs_epoll_ctl_fail =
    cs_eng("::epoll_ctl fail")
    cs_pl("błąd ::epoll_ctl");

core::String cs_epoll_wait_fail =
    cs_eng("::epoll_wait fail")
    cs_pl("błąd ::epoll_wait");
