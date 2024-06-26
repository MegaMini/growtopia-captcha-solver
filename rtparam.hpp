#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "../utils.h"

class rtvar {
public:
    class pair {
    public:
        std::string m_key;
        std::vector<std::string> m_values;
        
        pair() = default;

        pair(std::string key, std::initializer_list<std::string> values)
            : m_key(std::move(key)), m_values(values) { }

        bool operator==(const rtvar::pair& other) const {
            return m_key == other.m_key && m_values == other.m_values;
        }

        static pair parse(const std::string& str) {
            pair ret;
            if (str.empty()) {
                ret.m_values.push_back("[EMPTY]");
                return ret;
            }

            std::stringstream ss(str);
            std::string token;
            bool key = true;

            while (std::getline(ss, token, '|')) {
                if (key) {
                    ret.m_key = token;
                    key = false;
                } else {
                    ret.m_values.push_back(token);
                }
            }
            return ret;
        }

        std::string serialize() const {
            std::string ret = m_key;
            for (const auto& val : m_values) {
                ret.append("|").append(val);
            }
            return ret;
        }
    };

    rtvar() = default;
    
    rtvar(std::initializer_list<pair> pairs)
        : m_pairs(pairs) { }

    static rtvar parse(const std::string& str) {
        rtvar ret;
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, '\n')) {
            ret.append(token);
        }
        return ret;
    }

    pair& append(const std::string& str) {
        pair p = pair::parse(str);
        m_pairs.push_back(p);
        return m_pairs.back();
    }

    pair& get(size_t i) {
        if (i >= m_pairs.size()) {
            throw std::out_of_range("Index out of range");
        }
        return m_pairs[i];
    }

    bool valid() const {
        return !m_pairs.empty() && !m_pairs[0].m_values.empty();
    }

    pair* find(const std::string& key) {
        auto it = std::find_if(m_pairs.begin(), m_pairs.end(),
            [&key](const pair& p) { return p.m_key == key; });

        return it != m_pairs.end() ? &(*it) : nullptr;
    }

    std::string get(const std::string& key) const {
        auto it = std::find_if(m_pairs.begin(), m_pairs.end(),
            [&key](const pair& p) { return p.m_key == key; });

        return it != m_pairs.end() ? it->m_values[0] : "";
    }

    void set(const std::string& key, const std::string& value) {
        auto pair = find(key);
        if (pair && !pair->m_values.empty()) {
            pair->m_values[0] = value;
        }
    }

    std::string serialize() const {
        std::string ret;
        for (const auto& val : m_pairs) {
            ret.append(val.serialize()).append("\n");
        }
        if (!ret.empty()) {
            ret.pop_back();
        }
        return ret;
    }

    bool validate_ints(const std::vector<std::string>& vals) const {
        return std::all_of(vals.begin(), vals.end(),
            [this](const std::string& str) {
                const auto* pair = find(str);
                return pair && utils::is_number(pair->m_values[0]);
            });
    }

    bool validate_int(const std::string& str) const {
        const auto* pair = find(str);
        return pair && utils::is_number(pair->m_values[0]);
    }

    int get_int(const std::string& key) const {
        const auto* pair = find(key);
        if (!pair) {
            throw std::runtime_error("Key not found");
        }
        return std::stoi(pair->m_values[0]);
    }

    long long get_long(const std::string& key) const {
        const auto* pair = find(key);
        if (!pair) {
            throw std::runtime_error("Key not found");
        }
        return std::stoll(pair->m_values[0]);
    }

    size_t size() const {
        return m_pairs.size();
    }

    void remove(const std::string& key) {
        auto it = std::remove_if(m_pairs.begin(), m_pairs.end(),
            [&key](const pair& p) { return p.m_key == key; });
        m_pairs.erase(it, m_pairs.end());
    }

private:
    std::vector<pair> m_pairs;
};

class rtvar_opt {
private:
    std::string m_var;

public:
    rtvar_opt() = default;
    
    explicit rtvar_opt(std::string start)
        : m_var(std::move(start)) { }

    void append(const std::string& str) {
        m_var.append("\n").append(str);
    }

    std::string get() const {
        return m_var;
    }
};
