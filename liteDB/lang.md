```javascript

address: { street: string, house-no: int }

family: {
    last-name: string,
    first-name-seq: string[],
    address-seq: address[]
}

family-seq: family[]

family-seq << family {
    'Napiontek',
    ['Kris', 'Aga', 'Borys', 'Sophia'],
    [ { street: 'Southdene' }, { street: 'Warszawska' } ]
}

family-name-seq: family-seq[first-name[0] >= 'Z'].last-name

find-by-name(name: string): {
    name: family.last-name,
    street: family.address-seq[0]
} family-seq[last-name = 'Napiontek' & first-name-seq[name]]

hive-seq: find-by-name('Hive')

```

```c++

struct address {
    std::string street;
    int house_no;
};

struct family {
    std::string last_name;
    lite::db::Seq<std::string> first_name_seq;
    lite::db::Seq<address> address_seq;
};

lite::db::Seq<family> family_seq;

lite::db::Seq<std::string>> family_name_seq;

struct find_by_name_t {
    std::string name;
    std::string street;
};

lite::db::Seq<find_by_name_t> find_by_name(const std::string& name);

lite::db::Seq<find_by_name_t> hive_seq;

```
