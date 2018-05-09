#pragma once

#include "../Logging/Log.h"
#include <unordered_set>

namespace kiwi {

    class StringDatabase {
    private:
        using StringDBImpl = std::unordered_set<std::string>;

        StringDatabase() :
            _string_db(541)
        {};

    public:
        using StringStorage = std::string;
        using StringView = std::string_view;

        const StringView put(const StringStorage& str) {
            using insert_result = std::pair<StringDBImpl::iterator, bool>;
            insert_result result = _string_db.emplace(str);
            return StringView(*result.first);
        }

        bool contains(const StringStorage& str) const {
            using const_iterator = StringDBImpl::const_iterator;
            const_iterator data = _string_db.find(str);
            return data != _string_db.end();
        }

    private:
        std::unordered_set<StringStorage> _string_db;
    };
}