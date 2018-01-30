#include <unordered_map>
#include <memory>
#include <shared_mutex>
#include <cstring>
#include <string>
#include "tangledb.h"


class TangleDB {

public:

    struct TXRecordImpl {
        std::string hash;
        std::string trunk;
        std::string branch;

        TXRecordImpl() = default;
    };

    inline TXRecordImpl find(const std::string &hash);

    inline void put(const std::string &hash, const TXRecordImpl& tx);

    static TangleDB& instance();

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, TXRecordImpl> _txs;

    TangleDB() = default;
    ~TangleDB() = default;

    TangleDB(const TangleDB&) = delete;
    TangleDB& operator=(const TangleDB&) = delete;
    TangleDB(TangleDB&&) = delete;
    TangleDB& operator=(TangleDB&&) = delete;

};

TangleDB& TangleDB::instance(){
    static TangleDB db;
    return db;
}

//the problem with this function will be noticed when
//someone will keep its returned value as if it is a valid copy
//while this record has been somehow deleted
TangleDB::TXRecordImpl TangleDB::find(const std::string &hash) {

    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto tx = _txs.find(hash);
    if (tx != _txs.end()) {
        return tx->second;
    }
    return std::move(TXRecordImpl());
}

void TangleDB::put(const std::string &hash, const TXRecordImpl& tx) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    _txs[hash] = tx;
}

TXRecord findInTangleDB(const  char* hash){
    auto foundTX = std::move(TangleDB::instance().find(hash));
    return std::move(TXRecord{foundTX.hash.data(),foundTX.trunk.data(),foundTX.branch.data()});
}

void putInTangleDB(const char* hash, TXRecord tx){
    TangleDB::TXRecordImpl txImpl = {tx.hash,tx.trunk,tx.branch};
    TangleDB::instance().put(hash,txImpl);
}







