#include "api/rest.hpp"

namespace etwin::api {

RestApi::RestApi() {
    register_route("GET", "/state", [this](const RestRequest& r) { return handle_state(r); });
    register_route("GET", "/scenario", [this](const RestRequest& r) { return handle_scenario(r); });
    register_route("POST", "/scenario", [this](const RestRequest& r) { return handle_scenario(r); });
    register_route("GET", "/catalog", [this](const RestRequest& r) { return handle_catalog(r); });
    register_route("GET", "/provenance", [this](const RestRequest& r) { return handle_provenance(r); });
    register_route("GET", "/validation", [this](const RestRequest& r) { return handle_validation(r); });
}

void RestApi::register_route(const std::string& method, const std::string& path,
                             std::function<RestResponse(const RestRequest&)> handler) {
    routes_[method + " " + path] = std::move(handler);
}

RestResponse RestApi::handle(const RestRequest& request) const {
    std::string key = request.method + " " + request.path;
    auto it = routes_.find(key);
    if (it == routes_.end()) return not_found(request);
    return it->second(request);
}

RestResponse RestApi::handle_state(const RestRequest& request) const {
    RestResponse response;
    response.status_code = 200;
    response.content_type = "application/json";
    response.body = "{\"state\":\"ok\"}";
    return response;
}

RestResponse RestApi::handle_scenario(const RestRequest& request) const {
    RestResponse response;
    response.status_code = 200;
    response.content_type = "application/json";
    response.body = "{\"scenario\":\"ok\"}";
    return response;
}

RestResponse RestApi::handle_catalog(const RestRequest& request) const {
    RestResponse response;
    response.status_code = 200;
    response.content_type = "application/json";
    response.body = "{\"catalog\":\"ok\"}";
    return response;
}

RestResponse RestApi::handle_provenance(const RestRequest& request) const {
    RestResponse response;
    response.status_code = 200;
    response.content_type = "application/json";
    response.body = "{\"provenance\":\"ok\"}";
    return response;
}

RestResponse RestApi::handle_validation(const RestRequest& request) const {
    RestResponse response;
    response.status_code = 200;
    response.content_type = "application/json";
    response.body = "{\"validation\":\"ok\"}";
    return response;
}

RestResponse RestApi::not_found(const RestRequest& request) const {
    RestResponse response;
    response.status_code = 404;
    response.content_type = "application/json";
    response.body = "{\"error\":\"not found\"}";
    return response;
}

} // namespace etwin::api
