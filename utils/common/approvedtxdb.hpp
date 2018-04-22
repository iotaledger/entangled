#pragma once

#include <unordered_map>
#include <mutex>
#include <string>
#include <memory>


class ApprovedTXDB {
public:
    typedef std::unordered_map <std::string, std::string> ApproversMap;

    ApprovedTXDB();
    void put(const std::string &txHash, const std::string &approverHash);
    std::unique_ptr<ApproversMap> swapDB();
    //be careful with these, it stills _txsPtr
    ApprovedTXDB& operator=(ApprovedTXDB&);
    ApprovedTXDB& operator=(ApprovedTXDB&&);
private:
    mutable std::mutex _mutex;
    std::unique_ptr <ApproversMap> _txsPtr;
};