<img src="pic/novadb.svg" width = "300" alt="novadb"/>

[![](https://img.shields.io/badge/license-GPLv3-green)](https://github.com/Tencent/novadb/blob/master/LICENSE.txt) ![](https://img.shields.io/badge/Platform-Linux_x64-orange) [![](https://img.shields.io/badge/PRs-Welcome-orange)](https://github.com/Tencent/novadb/pulls)

novadb is a high-performance distributed storage system which is fully compatible with the Redis protocol. [中文文档](http://novadb.cn)

## Table of Contents
- [Introduction](#Introduction)
- [Features](#Features)
- [Get Started](#Get-Started)
  - [Requirements](#Requirements)
  - [Build](#Build)
  - [Run and Play](#Run-and-Play)
  - [Test](#Test)
- [Performance](#Performance)
  - [Hardware Spec](#Hardware-spec)
  - [Bechmarks and QPS numbers](#Bechmarks-and-QPS-numbers)
  - [QPS on different payload](#QPS-on-different-payload)
- [Contributing](#Contributing)
- [Support](#Support)
- [License](#License)
## Introduction

novadb is a high-performance distributed storage system which is fully compatible with the Redis protocol. It uses RocksDB as the storage engine, and all data is stored to disks through RocksDB. Users can access novadb using a Redis client, and the application hardly needs to be changed. In addition, novadb supports storage capacity far exceeding memory, which can greatly reduce user storage costs.

Similar to Redis clusters, novadb uses a decentralized distributed solution. The gossip protocol is used for communication between nodes, and all nodes in a cluster can be routed to the correct node when a user accesses. Cluster nodes support automatic discovery of other nodes, detect faulty nodes, and ensure the application is almost not affected when the master node failed.

## Features

- Redis compatibility

  Redis protocol and commands supported in novadb are compatible with Redis.

- Persistent storage

  Using RocksDB as storage engine. All data is stored in RocksDB in a specific format, supporting PB-level storage capacity.

- Decentralized distributed cluster

  Distributed implementation like Redis clusters, using a gossip protocol to intercommunicate between nodes.

- Horizontal scalability

  Data migration online between nodes. High performance and linear scalability up to 1,000 nodes.

- Failover

  Auto-detect non-working nodes, and promote replica nodes to master when a failure occurs.

- Key component for novadb Hybrid Storage Edition

  Thanks to the design and internal optimization, Redis and novadb can work together to be Hybrid Storage Edition. It is suitable for KV storage scenarios, as it balances performance and cost, and greatly reduces your business operating costs by 80% in the scenarios where cold data takes up a lot of storage space.

## Get Started

### Requirements

* g++ (required by c++17, version >= 5.5)
* cmake (version >= 3.13.0)

### Build

```
$ git clone https://github.com/Tencent/novadb.git --recursive
$ git submodule update --init --recursive
$ mkdir bulid
$ cd build & cmake ..
$ make -j12
```

### Run and Play
```
$ ./build/bin/novadbplus novadbplus.conf
```
Connect to the server via redis-cli
```
$ redis-cli -p 51002
```

### TEST
You can make some change to the code and make sure the following test  script pass.
```
$ sh ./testall.sh
```

## Performance

### Hardware spec

```
CPU:2.50 GHz,48 core
DISK:NVMe SSD
MEM:192GB
```

### Benchmarks and QPS numbers

novadbplus: workers = 56
```
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="set __key__ __data__" --key-prefix="kv_" --key-minimum=1 --key-maximum=500000000 --random-data --data-size=128 --test-time=1800
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="get __key__" --key-prefix="kv_" --key-minimum=1 --key-maximum=500000000 --test-time=1800
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="incr __key__" --key-prefix="int_" --key-minimum=1 --key-maximum=1000000 --test-time=1800
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="lpush __key__ __data__" --key-prefix="list_" --key-minimum=1 --key-maximum=1000000 --random-data --data-size=128 --test-time=1800
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="sadd __key__ __data__" --key-prefix="set_" --key-minimum=1 --key-maximum=1000000 --random-data --data-size=128 --test-time=1800
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="zadd __key__ __key__ __data__" --key-prefix="" --key-minimum=1 --key-maximum=1000000 --random-data --data-size=128 --test-time=1800
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="hset __key__ __data__ __data__" --key-prefix="hash_" --key-minimum=1 --key-maximum=1000000 --random-data --data-size=128 --test-time=1800
```
   ![image.png](pic/qps.png)

latency:
```
SET    1.6ms    99%
SET    2.2ms    99.9%
SET    6.9ms    99.99%
SET    9.4ms    100.00%
```

###  QPS on different payload

novadbplus: workers = 56
```
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="set __key__ __data__" --key-prefix="kv_" --command-key-pattern=R --random-data --data-size=128 --test-time=1800
./memtier_benchmark -t 20 -c 50 -s 127.0.0.1 -p 51002 --distinct-client-seed --command="get __key__" --key-prefix="kv_" --command-key-pattern=R --test-time=1800
```
![image.png](pic/qps_payload.png)

we test set for half an hour, and then test get key half an hour. because the data is not big enough, most of the data is in memory, so the get qps for different payload is nearly the same.

## Contributing

For more information regarding contributing issues or pull requests, checkout [CONTRIBUTING](CONTRIBUTING.md)

## Support

Checkout [support](http://novadb.cn/#/support) for FAQs or join our discussion groups.

## License

novadb is licensed under the GNU General Public License Version 3.0. Copyright and license information can be found in the file [LICENSE.txt](LICENSE.txt).
