# Distributed Database Draft

## Protocol
 - TCP makes sure that messages are received in order only between 2 engines.
 - It is not true for 3 engines due to different latencies.
Multicast/UDP is a good answer since we have to sort messages anyway.

## Topology
 - Connections everyone to everyone is an overkill thus a tree topology should be used. It requires relaying massages. A -> B -> C. B relays massages from A to C. Multicast could be the best answer here.

## Data Log
 - Engines exchage messages (put, get, change, metadata, metadata-operations). Eventually all engines should contain exactly the same state of data. In-memory engines can loose data and be able to retrieve it from other engines. Log of messages is persistent and used for storage to create consistent data.

## Catalog
 - Catalog is a piece of meta-data which tells a engine where to find real data or meta-data pointing to yet another data. Catalog can be structured as tree. Root catalog must be replicated to all possible engines. Catalog leaves may be replicated when data is requested. It can be recursive engine.
 - Catalog Types:
   * Schema Catalog
   * Sequence Catalog
   * Record Catalog
   * Data Catalog
   * Index Catalog

## Distributed Blocks
 - Atoms can contain any type of data. Their task is to replicate data to other engines. When atom is changed - its engine makes sure that the change was replicated to all other engines which hold him. Atom knows all engines holding him. Atom knows its originator, if originating engine is dropped the next one on the sorted list of engines should be chosen immediately.

## Data Split
 - Single engine can contain as much data as it needs. It can aqcuire missing data from other engines and drop it when it is not longer needed. Engine looks-up at catalog where to find data.
 - Size of data chunk (atom) should be big enough to make possible to make operations on it. It should be small enough to send it to another engine when requested. 

## Storage
 - Storage should contain all data.

## Garbage Collector
 - GC walks thru all serialiazed data and marks those not referenced by any transaction

## Data Removal
 - Single piece of data can be marked as removed and be not visible to other transactions. When GC finds out that there is not single reference to this piece of data then it can be removed pernamentaly.

## Data Representation Model
 - Model of data should follow relational database (SQL). It is the most flexible model. Key-Value hash maps can make serious bottlenecks - second indices, updates of one column etc.

## jemalloc vc libc vc lock-free

## Engine Types: Cache (in-memory) and Archive (persistent with journal log)

## Data Block Request between Engines
 - Engine-A creates Sequence-A and publish this information in Root Catalog
 - Engine-B finds in catalogs that Engine-A holds meta-data of Sequence-A
 - Engine-B requests Sequence-A from Engine-A
 - Sequence-A holds list of engines [Engine-A, Engine-B].
 - Engine-A is an originator of Sequence-A and always much be requested for permission to modify Sequence-A
 - If Engine-A is not available than next engine in the engine list Engine-B immediately becomes leader of Sequence-A

