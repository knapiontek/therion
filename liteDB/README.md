# Distributed Database Draft

## Protocol
 - TCP makes sure that messages are received in order only between 2 processes.
 - It is not true for 3 processes due to different latencies.
Multicast/UDP is a good answer since we have to sort messages anyway.

## Topology
 - Connections everyone to everyone is an overkill thus a tree topology should be used. It requires relaying massages. A -> B -> C. B relays massages from A to C. Multicast could be the best answer here.

## Data Log
 - Processes exchage messages (put, get, change, metadata, metadata-operations). Eventually all processes should contain exactly the same state of data. In-memory processes can loose data and be able to retrieve it from other processes. Log of messages is persistent and used for storage to create consistent data.

## Catalog
 - Catalog is a piece of metadata which tells a process when to find real data. Catalog can be structured as tree. Root catalog must be replicated to all possible processes. 

## Distributed Objects: Atoms
 - Atoms can contain any type of data. Their task is to replicate data to other processes. When atom is changed - its process makes sure that the change was replicated to all other processes which hold him. Atom knows all processes holding him. Atom knows its originator, if originating process is dropped the next one on the sorted list of processes should be chosen immediately.

## Data Split
 - Single process can contain as much data as it needs. It can aqcuire missing data from other processes and drop it when it is not longer needed. Process looks-up at catalog where to find data.
 - Size of data chunk (atom) should be big enough to make possible to make operations on it. It should be small enough to send it to another process when requested. 

## Storage
 - Storage should contain all data.

## Garbage Collector
 - GC walks thru all serialiazed data and marks those not referenced by any transaction

## Data removal
 - Single piece of data can be marked as removed and be not visible to other transactions. When GC finds out that there is not single reference to this piece of data then it can be removed pernamentaly.

## Data Representation Model
 - Model of data should follow relational database (SQL). It is the most flexible model. Key-Value hash maps can make serious bottlenecks - second indices, updates of one column etc.
