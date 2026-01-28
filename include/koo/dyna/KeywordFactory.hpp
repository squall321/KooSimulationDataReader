#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace koo::dyna {

// Keyword creator function type
using KeywordCreator = std::function<std::unique_ptr<Keyword>()>;

/**
 * @brief Keyword factory - creates keyword objects by name
 *
 * Singleton pattern with self-registering keywords.
 * New keywords register themselves at static initialization time.
 */
class KOO_API KeywordFactory {
public:
    // Singleton access
    static KeywordFactory& instance();

    // Register a keyword creator
    bool registerKeyword(const std::string& name, KeywordCreator creator);

    // Create keyword by name
    std::unique_ptr<Keyword> create(const std::string& name) const;

    // Check if keyword is registered
    bool isRegistered(const std::string& name) const;

    // Get all registered keyword names
    std::vector<std::string> getRegisteredKeywords() const;

    // Get keyword count
    size_t getKeywordCount() const { return creators_.size(); }

private:
    KeywordFactory() = default;
    KeywordFactory(const KeywordFactory&) = delete;
    KeywordFactory& operator=(const KeywordFactory&) = delete;

    std::unordered_map<std::string, KeywordCreator> creators_;
};

/**
 * @brief Auto-registration helper
 *
 * Use REGISTER_KEYWORD macro instead of using this directly.
 */
template<typename T>
class KeywordRegistrar {
public:
    explicit KeywordRegistrar(const std::string& name) {
        KeywordFactory::instance().registerKeyword(name, []() {
            return std::make_unique<T>();
        });
    }
};

} // namespace koo::dyna

/**
 * @brief Macro for automatic keyword registration
 *
 * Usage in .cpp file:
 * REGISTER_KEYWORD(MatElastic, "*MAT_ELASTIC")
 * REGISTER_KEYWORD(MatElastic, "*MAT_001")  // Alias
 */
#define KOO_CONCAT_IMPL(a, b) a##b
#define KOO_CONCAT(a, b) KOO_CONCAT_IMPL(a, b)
#define KOO_UNIQUE_NAME(base) KOO_CONCAT(base, __COUNTER__)

#define REGISTER_KEYWORD(ClassName, KeywordName)                              \
    namespace {                                                                \
        static const ::koo::dyna::KeywordRegistrar<ClassName>                 \
            KOO_UNIQUE_NAME(_koo_reg_)(KeywordName);                          \
    }
