#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "state/earth_state.hpp"
#include "scenario/scenario_engine.hpp"

namespace etwin::api {

struct RestRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> query_params;
    std::string body;
};

struct RestResponse {
    int status_code;
    std::string content_type;
    std::string body;
};

class RestApi {
public:
    RestApi();

    RestResponse handle(const RestRequest& request) const;

    void register_route(const std::string& method, const std::string& path,
                        std::function<RestResponse(const RestRequest&)> handler);

private:
    std::map<std::string, std::function<RestResponse(const RestRequest&)>> routes_;

    RestResponse handle_state(const RestRequest& request) const;
    RestResponse handle_scenario(const RestRequest& request) const;
    RestResponse handle_catalog(const RestRequest& request) const;
    RestResponse handle_provenance(const RestRequest& request) const;
    RestResponse handle_validation(const RestRequest& request) const;
    RestResponse not_found(const RestRequest& request) const;
};

} // namespace etwin::api
