```javascript

address: { street: string, house-no: int }

family: {
    last-name: string,
    first-name-seq: string[],
    address-seq: address[]
}

family-seq: family[]

family-seq << family { 'Napiontek', ['Kris', 'Aga', 'Borys', 'Sophia'], [ { street: 'Southdene' }, { street: 'Warszawska' } ] }

family-name-seq: family-seq[first-name[0] >= 'Z'].last-name

find-by-name(name: string): { name: family.last-name, street: family.address-seq[0] } family-seq[last-name = 'Napiontek' & first-name-seq[name]]

hive-seq: find-by-name('Hive')

```

```c++

class address { std::string; int house-no; }

class family {
    std::string last-name;
    std::vector<std::string> first-name-seq;
    std::vector<address> address-seq;
}

```
