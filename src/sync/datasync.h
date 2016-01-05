#ifndef datasync_h
#define datasync_h

#include <vector>

class SyncNode
{
    int rank;
    std::vector<std::string> hosts;

    void bcast() {
        // TODO: Send data to other hosts from this.
    }

    void hearthbeat() {
        // TODO: Ping hosts.
    }

    void init(const char *hostsEnv) {
        // TODO: Split hosts variables, get rank.
    }

public:
    SyncNode(const char *_hosts) {
        init(_hosts);
    }
};

#endif
