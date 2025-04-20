#include "redirect.hpp"

#include <fmt/format.h>
#include <string_view>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/utils/assert.hpp>
#include "redirect.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/serialize.hpp"
#include "userver/server/http/http_status.hpp"

namespace url_shortener {

Redirect::Redirect(const components::ComponentConfig& config,
                   const components::ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      pg_cluster_(
          component_context.FindComponent<components::Postgres>("postgres-db-1")
              .GetCluster()) {}

std::string Redirect::HandleRequestThrow(
    const server::http::HttpRequest& request,
    server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType("application/json");

  const std::string& short_uri = request.GetPathArg("short_uri");
  if (!short_uri.empty()) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT url FROM shortener_schema.urls "
        "WHERE short_uri = $1",
        short_uri);

    if (auto url = result.AsSingleRow<std::string>(); !url.empty()) {
      std::string locationHeaderName = "Location";
      response.SetStatus(server::http::HttpStatus::kFound);
      response.SetHeader(locationHeaderName, url);

      auto json_response = userver::formats::json::MakeObject(
          "message", "forwarded to target url");
      return userver::formats::json::ToString(json_response);
    }
  } else {
    response.SetStatus(server::http::HttpStatus::kBadRequest);
    auto json_response = userver::formats::json::MakeObject(
        "message", "short_uri argument not provided");
    return userver::formats::json::ToString(json_response);
  }
  return "";
}

}  // namespace url_shortener
