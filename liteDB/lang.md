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
    string street;
    int house_no;
};

struct family {
    string last_name;
    vector<string> first_name_seq;
    vector<address> address_seq;
};

vector<family> family_seq = family {
    'Napiontek',
    vector<string> { 'Kris', 'Aga', 'Borys', 'Sophia' },
    vector<address> { { street: 'Southdene' }, { street: 'Warszawska' } }
};

vector<string>> family_name_seq;

vector<find_by_name_t> find_by_name(const string& name);

for (auto &i: find_by_name('Hello')) {
    cout << "name: " << i.name << "street: " << i.street << endl;
}

vector<find_by_name_t> hive_seq('Hive');

```
