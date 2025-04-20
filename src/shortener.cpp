#include "shortener.hpp"

#include <fmt/format.h>
#include <random>
#include <string_view>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/utils/assert.hpp>
#include "redirect.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/serialize.hpp"

namespace url_shortener {

Shortener::Shortener(const components::ComponentConfig& config,
                     const components::ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      pg_cluster_(
          component_context.FindComponent<components::Postgres>("postgres-db-1")
              .GetCluster()) {}

std::string Shortener::HandleRequestThrow(
    const server::http::HttpRequest& request,
    server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType("application/json");

  const std::string& target_url = request.GetArg("url");
  std::string short_uri = GenerateShortURI();
  const auto now = std::chrono::time_point_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now());

  if (!target_url.empty()) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO shortener_schema.urls(url, short_uri, created_at) "
        "VALUES ($1, $2, $3) RETURNING urls.short_uri",
        target_url, short_uri,
        userver::storages::postgres::TimePointWithoutTz{now});

    if (!result.AsSingleRow<std::string>().empty()) {
      UASSERT(short_uri == result.AsSingleRow<std::string>());
    }
  } else {
    // TODO: return 400: no target url
  }

  // TODO: Replace string literal to port value from config
  auto json_response = userver::formats::json::MakeObject(
      "url", fmt::format("http://localhost:{}/{}", "8080", short_uri));
  return userver::formats::json::ToString(json_response);
}

std::string GenerateShortURI() {
  static std::string possible_chars =
      "0123456789"
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  static std::random_device rd;
  static std::minstd_rand gen(rd());
  static std::uniform_int_distribution<> rnd(0, possible_chars.size() - 1);
  constexpr std::size_t short_path_size = 6;

  std::string result;
  for (std::size_t i = 0; i < short_path_size; ++i) {
    result += possible_chars[rnd(gen)];
  }
  return result;
}

void AppendShortener(userver::components::ComponentList& component_list) {
  component_list.Append<url_shortener::Shortener>();
  component_list.Append<url_shortener::Redirect>();
  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();
}

}  // namespace url_shortener
