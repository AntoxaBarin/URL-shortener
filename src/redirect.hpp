#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include "userver/server/http/http_response.hpp"

using namespace userver;

namespace url_shortener {

class Redirect final : public server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-redirect";

  Redirect(const components::ComponentConfig& config,
            const components::ComponentContext& component_context);

  std::string HandleRequestThrow(
      const server::http::HttpRequest& request,
      server::request::RequestContext&) const override;

 private:
  storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace url_shortener
