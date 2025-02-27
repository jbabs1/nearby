// Copyright 2023 Google LLC
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

#ifndef THIRD_PARTY_NEARBY_FASTPAIR_KEYED_SERVICE_FAST_PAIR_MEDIATOR_H_
#define THIRD_PARTY_NEARBY_FASTPAIR_KEYED_SERVICE_FAST_PAIR_MEDIATOR_H_

#include <memory>

#include "fastpair/scanning/scanner_broker.h"
#include "fastpair/server_access/fast_pair_repository.h"

namespace nearby {
namespace fastpair {

// Implements the Mediator design pattern for the components in the Fast Pair
class Mediator final : public ScannerBroker::Observer {
 public:
  Mediator(std::unique_ptr<ScannerBroker> scanner_broker,
           std::unique_ptr<FastPairRepository> fast_pair_repository);
  Mediator(const Mediator&) = delete;
  Mediator& operator=(const Mediator&) = delete;
  ~Mediator() override = default;

  // ScannerBroker::Observer
  void OnDeviceFound(const FastPairDevice& device) override;
  void OnDeviceLost(const FastPairDevice& device) override;

  void StartScanning();

 private:
  bool IsFastPairEnabled();

  std::unique_ptr<ScannerBroker> scanner_broker_;
  std::unique_ptr<FastPairRepository> fast_pair_repository_;
};

}  // namespace fastpair
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_FASTPAIR_KEYED_SERVICE_FAST_PAIR_MEDIATOR_H_
