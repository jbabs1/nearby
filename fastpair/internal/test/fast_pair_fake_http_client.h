// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THIRD_PARTY_NEARBY_FASTPAIR_INTERNAL_TEST_FAST_PAIR_FAKE_HTTP_CLIENT_H_
#define THIRD_PARTY_NEARBY_FASTPAIR_INTERNAL_TEST_FAST_PAIR_FAKE_HTTP_CLIENT_H_

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/status/statusor.h"
#include "internal/network/http_client.h"

namespace nearby {
namespace network {

class FastPairFakeHttpClient : public HttpClient {
 public:
  struct RequestInfo {
    HttpRequest request;
    std::function<void(const absl::StatusOr<HttpResponse>&)> callback;
  };

  FastPairFakeHttpClient() = default;
  explicit FastPairFakeHttpClient(const HttpRequest& request);
  ~FastPairFakeHttpClient() override = default;

  FastPairFakeHttpClient(const FastPairFakeHttpClient&) = default;
  FastPairFakeHttpClient& operator=(const FastPairFakeHttpClient&) = default;
  FastPairFakeHttpClient(FastPairFakeHttpClient&&) = default;
  FastPairFakeHttpClient& operator=(FastPairFakeHttpClient&&) = default;

  void StartRequest(const HttpRequest& request,
                    std::function<void(const absl::StatusOr<HttpResponse>&)>
                        callback) override {
    RequestInfo request_info;
    request_info.request = request;
    request_info.callback = callback;
    request_infos_.push_back(std::move(request_info));
  }

  absl::StatusOr<HttpResponse> GetResponse(
      const HttpRequest& request) override {
    return absl::UnimplementedError("unimplemented");
  }

  // Mock methods
  void CompleteRequest(const absl::StatusOr<HttpResponse>& response,
                       size_t pos = 0) {
    if (pos >= request_infos_.size()) {
      return;
    }

    auto request_info = request_infos_.at(pos);
    if (request_info.callback != nullptr) {
      request_info.callback(response);
    }

    request_infos_.erase(request_infos_.begin() + pos);
  }

  void CompleteRequest(
      int error, std::optional<HttpStatusCode> response_code = std::nullopt,
      const std::optional<std::string>& response_string = std::nullopt,
      size_t pos = 0) {
    absl::StatusOr<HttpResponse> result;

    HttpResponse response;
    if (error == 0) {
      absl::Status status =
          HTTPCodeToStatus(static_cast<int>(*response_code), "");
      if (status.ok()) {
        if (response_code.has_value()) {
          response.SetStatusCode(*response_code);
        }

        if (response_string.has_value()) {
          response.SetBody(*response_string);
        }

        response.SetHeaders({{"Content-type", {"text/html"}}});
        result = response;
      } else {
        result = status;
      }
    } else {
      result = absl::Status(absl::StatusCode::kFailedPrecondition,
                            std::to_string(error));
    }

    CompleteRequest(result, pos);
  }

  absl::Status HTTPCodeToStatus(int status_code,
                                absl::string_view status_message) {
    absl::Status status = absl::OkStatus();
    switch (status_code) {
      case 400:
        return absl::InvalidArgumentError(status_message);
      case 401:
        return absl::UnauthenticatedError(status_message);
      case 403:
        return absl::PermissionDeniedError(status_message);
      case 404:
        return absl::NotFoundError(status_message);
      case 409:
        return absl::AbortedError(status_message);
      case 416:
        return absl::OutOfRangeError(status_message);
      case 429:
        return absl::ResourceExhaustedError(status_message);
      case 499:
        return absl::CancelledError(status_message);
      case 504:
        return absl::DeadlineExceededError(status_message);
      case 501:
        return absl::UnimplementedError(status_message);
      case 503:
        return absl::UnavailableError(status_message);
      default:
        break;
    }
    if (status_code >= 200 && status_code < 300) {
      return absl::OkStatus();
    } else if (status_code >= 400 && status_code < 500) {
      return absl::FailedPreconditionError(status_message);
    } else if (status_code >= 500 && status_code < 600) {
      return absl::InternalError(status_message);
    }
    return absl::UnknownError(status_message);
  }

  const std::vector<RequestInfo>& GetPendingRequest() { return request_infos_; }

 private:
  std::vector<RequestInfo> request_infos_;
};

}  // namespace network
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_FASTPAIR_INTERNAL_TEST_FAST_PAIR_FAKE_HTTP_CLIENT_H_
