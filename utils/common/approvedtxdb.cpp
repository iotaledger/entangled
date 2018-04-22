#include "approvedtxdb.hpp"


ApprovedTXDB::ApprovedTXDB() {
    _txsPtr = std::make_unique<ApprovedTXDB::ApproversMap>();
}

ApprovedTXDB& ApprovedTXDB::operator=(ApprovedTXDB& other){
    _txsPtr = std::move(other._txsPtr);
    return *this;
}

ApprovedTXDB& ApprovedTXDB::operator=(ApprovedTXDB&& other){
    _txsPtr = std::move(other._txsPtr);
    return *this;
}

std::unique_ptr<ApprovedTXDB::ApproversMap> ApprovedTXDB::swapDB(){
    std::lock_guard<std::mutex> guard(_mutex);
    auto old = std::move(_txsPtr);
    _txsPtr = std::make_unique<ApprovedTXDB::ApproversMap>();
    return old;
}


void ApprovedTXDB::put(const std::string &txHash, const std::string &approverHash) {
    std::lock_guard<std::mutex> guard(_mutex);
    auto& txs = *_txsPtr;
    txs[txHash] = approverHash;
}



