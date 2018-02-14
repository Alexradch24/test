/**************************************************************************
   Copyright (c) 2017 sewenew

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *************************************************************************/

#ifndef SEWENEW_REDISPLUSPLUS_CONNECTION_POOL_H
#define SEWENEW_REDISPLUSPLUS_CONNECTION_POOL_H

#include <chrono>
#include <mutex>
#include <deque>
#include "connection.h"

namespace sw {

namespace redis {

struct ConnectionPoolOptions {
    // Max number of connections, including both in-use and idle ones.
    std::size_t size = 1;

    // Max time to wait for a connection. 0ms means client waits forever.
    std::chrono::steady_clock::duration wait_timeout{};

    // Max lifetime of a connection. 0ms means we never expire the connection.
    std::chrono::steady_clock::duration connection_lifetime{};
};

class ConnectionPool {
public:
    explicit ConnectionPool(const ConnectionPoolOptions &pool_opts,
            const ConnectionOptions &connection_opts);

    Connection fetch();

    void release(Connection connection);

private:
    // NOT thread-safe
    Connection _fetch();

    void _wait_for_connection(std::unique_lock<std::mutex> &lock);

    bool _need_reconnect(const Connection &connection);

    Connector _connector;

    ConnectionPoolOptions _pool_opts;

    std::deque<Connection> _pool;

    std::mutex _mutex;

    std::condition_variable _cv;
};

}

}

#endif // end SEWENEW_REDISPLUSPLUS_CONNECTION_POOL_H