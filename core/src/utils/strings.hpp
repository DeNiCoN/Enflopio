#pragma once
#include <string>
#include <vector>

namespace Enflopio
{
    namespace utils
    {
        template<typename C>
        auto split(C&& str, const std::string& delimiter)
        {
            std::vector<typename std::remove_reference<C>::type> strings;

            for (auto p = str.data(), end = p + str.length();
                 p != end;
                 p += ((p==end) ? 0 : delimiter.length()) )
            {
                const auto pre = p;
                p = std::search(pre, end, delimiter.cbegin(), delimiter.cend());

                if (p != pre)
                {
                    strings.emplace_back(pre, p - pre);
                }
            }

            return strings;
        }

    }
}
