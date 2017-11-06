#pragma once

#include <memory>
#include <rx.hpp>
#include <statscollector/iri.hpp>

void zmqPublisher(rxcpp::subscriber<std::shared_ptr<iri::IRIMessage>>, const std::string& uri);
