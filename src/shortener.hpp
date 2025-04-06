#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace url_shortener {

class Shortener final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-shortener";

  Shortener(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& component_context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

std::string GenerateShortURI();

void AppendShortener(userver::components::ComponentList& component_list);

}  // namespace url_shortener
