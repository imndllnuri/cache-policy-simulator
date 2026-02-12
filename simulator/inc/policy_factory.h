#pragma once

#include "../replacement/replacement_policy.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class PolicyFactory {
public:
  using Creator = std::function<std::unique_ptr<ReplacementPolicy>(
      size_t sets, size_t ways)>;

  static PolicyFactory &instance();

  void register_policy(const std::string &name, Creator creator);
  std::unique_ptr<ReplacementPolicy> create(const std::string &name,
                                            size_t sets, size_t ways);

private:
  PolicyFactory() = default;
  std::unordered_map<std::string, Creator> registry_;
};
