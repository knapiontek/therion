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

# dot on seq '[].' creates new sequence

find-by-name(name: string):
    family-seq[last-name = 'Napiontek' & first-name-seq[name]]
        .address-seq[street = 'Southdene']
{
    street: .street, # number of dots in agregator must match those in selector
    house-no: address.house-no, # another way to choose part of selector
    family-name: last-name
}

hive-seq: find-by-name('Hive')

###

person: { name: string, address: { street: string } }

employee: person { work: string }

find-people(name: string): person[] {
    var test: string
    . << person[]
}

hello: find-people("Kris")

find-address(name: string): person[] {
    for i: people[name = "Kris"] {
        . << i.address
    }
}

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
