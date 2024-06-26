#pragma once
#include <stdint.h>
#include <sstream>
#include <cstring>
#include <string>
#include "vector.hpp"

#define C_VAR_SPACE_BYTES 16

class variant_t {
public:
    enum class vartype_t { TYPE_UNUSED, TYPE_FLOAT, TYPE_STRING, TYPE_VECTOR2, TYPE_VECTOR3, TYPE_UINT32, TYPE_ENTITY, TYPE_COMPONENT, TYPE_RECT, TYPE_INT32 };

    variant_t() : m_type(vartype_t::TYPE_UNUSED), m_pointer(nullptr) {}
    variant_t(const variant_t& v) { *this = v; }
    variant_t(int32_t var) : variant_t() { set(var); }
    variant_t(uint32_t var) : variant_t() { set(var); }
    variant_t(float var) : variant_t() { set(var); }
    variant_t(float x, float y) : variant_t() { set(vector2_t(x, y)); }
    variant_t(float x, float y, float z) : variant_t() { set(vector3_t(x, y, z)); }
    variant_t(const vector2_t& v2) : variant_t() { set(v2); }
    variant_t(const vector3_t& v3) : variant_t() { set(v3); }
    variant_t(const rect_t& r) : variant_t() { set(r); }
    variant_t(const std::string& var) : variant_t() { set(var); }

    void reset() { m_type = vartype_t::TYPE_UNUSED; }

    void set(const variant_t& v) {
        switch (v.get_type()) {
        case vartype_t::TYPE_FLOAT: set(v.get_float()); break;
        case vartype_t::TYPE_STRING: set(v.get_string()); break;
        case vartype_t::TYPE_VECTOR2: set(v.get_vector2()); break;
        case vartype_t::TYPE_VECTOR3: set(v.get_vector3()); break;
        case vartype_t::TYPE_UINT32: set(v.get_uint32()); break;
        case vartype_t::TYPE_INT32: set(v.get_int32()); break;
        case vartype_t::TYPE_RECT: set(v.get_rect()); break;
        case vartype_t::TYPE_ENTITY:
        case vartype_t::TYPE_COMPONENT:
        default: break;
        }
    }

    void set(float var) { m_type = vartype_t::TYPE_FLOAT; *reinterpret_cast<float*>(m_var) = var; }
    void set(uint32_t var) { m_type = vartype_t::TYPE_UINT32; *reinterpret_cast<uint32_t*>(m_var) = var; }
    void set(int32_t var) { m_type = vartype_t::TYPE_INT32; *reinterpret_cast<int32_t*>(m_var) = var; }
    void operator=(float var) { set(var); }
    void operator=(int32_t var) { set(var); }
    void operator=(uint32_t var) { set(var); }
    void operator=(const std::string& var) { set(var); }
    void set(const std::string& var) { m_type = vartype_t::TYPE_STRING; m_string = var; }
    void operator=(const vector2_t& var) { set(var); }
    void set(const vector2_t& var) { m_type = vartype_t::TYPE_VECTOR2; *reinterpret_cast<vector2_t*>(m_var) = var; }
    void set(float x, float y) { set(vector2_t(x, y)); }
    void operator=(const vector3_t& var) { set(var); }
    void operator=(const rect_t& var) { set(var); }
    void set(const vector3_t& var) { m_type = vartype_t::TYPE_VECTOR3; *reinterpret_cast<vector3_t*>(m_var) = var; }
    void set(const rect_t& var) { m_type = vartype_t::TYPE_RECT; *reinterpret_cast<rect_t*>(m_var) = var; }
    void set(float x, float y, float z) { set(vector3_t(x, y, z)); }

    float& get_float() { if (m_type == vartype_t::TYPE_UNUSED) set(float(0)); return *reinterpret_cast<float*>(m_var); }
    int32_t& get_int32() { if (m_type == vartype_t::TYPE_UNUSED) set(int32_t(0)); return *reinterpret_cast<int32_t*>(m_var); }
    uint32_t& get_uint32() { if (m_type == vartype_t::TYPE_UNUSED) set(uint32_t(0)); return *reinterpret_cast<uint32_t*>(m_var); }
    std::string& get_string() { return m_string; }
    vector2_t& get_vector2() { if (m_type == vartype_t::TYPE_UNUSED) set(vector2_t(0, 0)); return *reinterpret_cast<vector2_t*>(m_var); }
    vector3_t& get_vector3() { if (m_type == vartype_t::TYPE_UNUSED) set(vector3_t(0, 0, 0)); return *reinterpret_cast<vector3_t*>(m_var); }
    rect_t& get_rect() { if (m_type == vartype_t::TYPE_UNUSED) set(rect_t(0, 0, 0, 0)); return *reinterpret_cast<rect_t*>(m_var); }

    const float& get_float() const { return *reinterpret_cast<const float*>(m_var); }
    const int32_t& get_int32() const { return *reinterpret_cast<const int32_t*>(m_var); }
    const uint32_t& get_uint32() const { return *reinterpret_cast<const uint32_t*>(m_var); }
    const std::string& get_string() const { return m_string; }
    const vector2_t& get_vector2() const { return *reinterpret_cast<const vector2_t*>(m_var); }
    const vector3_t& get_vector3() const { return *reinterpret_cast<const vector3_t*>(m_var); }
    const rect_t& get_rect() const { return *reinterpret_cast<const rect_t*>(m_var); }

    vartype_t get_type() const { return m_type; }

    std::string print() const {
        switch (get_type()) {
        case vartype_t::TYPE_FLOAT: return std::to_string(get_float());
        case vartype_t::TYPE_STRING: return get_string();
        case vartype_t::TYPE_VECTOR2: return "x: " + std::to_string(get_vector2().m_x) + " y: " + std::to_string(get_vector2().m_y);
        case vartype_t::TYPE_VECTOR3: return "x: " + std::to_string(get_vector3().m_x) + " y: " + std::to_string(get_vector3().m_y) + " z: " + std::to_string(get_vector3().m_z);
        case vartype_t::TYPE_UINT32: return std::to_string(get_uint32());
        case vartype_t::TYPE_INT32: return std::to_string(get_int32());
        case vartype_t::TYPE_RECT: return "x: " + std::to_string(get_rect().m_x) + " y: " + std::to_string(get_rect().m_y) + " w: " + std::to_string(get_rect().m_w) + " h: " + std::to_string(get_rect().m_h);
        case vartype_t::TYPE_ENTITY:
        case vartype_t::TYPE_COMPONENT:
        case vartype_t::TYPE_UNUSED: return "unused";
        default: return "unknown";
        }
    }

    variant_t& operator=(const variant_t& rhs) {
        if (this != &rhs) {
            m_type = rhs.m_type;
            m_pointer = rhs.m_pointer;
            std::memcpy(m_var, rhs.m_var, C_VAR_SPACE_BYTES);
            m_string = rhs.m_string;
        }
        return *this;
    }

    variant_t& operator+=(const variant_t& rhs) {
        if (get_type() == rhs.get_type()) {
            switch (get_type()) {
            case vartype_t::TYPE_FLOAT: set(get_float() + rhs.get_float()); break;
            case vartype_t::TYPE_STRING: set(get_string() + rhs.get_string()); break;
            case vartype_t::TYPE_VECTOR2: set(get_vector2() + rhs.get_vector2()); break;
            case vartype_t::TYPE_VECTOR3: set(get_vector3() + rhs.get_vector3()); break;
            case vartype_t::TYPE_UINT32: set(get_uint32() + rhs.get_uint32()); break;
            case vartype_t::TYPE_INT32: set(get_int32() + rhs.get_int32()); break;
            default: break;
            }
        }
        return *this;
    }

    variant_t& operator-=(const variant_t& rhs) {
        if (get_type() == rhs.get_type()) {
            switch (get_type()) {
            case vartype_t::TYPE_FLOAT: set(get_float() - rhs.get_float()); break;
            case vartype_t::TYPE_VECTOR2: set(get_vector2() - rhs.get_vector2()); break;
            case vartype_t::TYPE_VECTOR3: set(get_vector3() - rhs.get_vector3()); break;
            case vartype_t::TYPE_UINT32: set(get_uint32() - rhs.get_uint32()); break;
            case vartype_t::TYPE_INT32: set(get_int32() - rhs.get_int32()); break;
            default: break;
            }
        }
        return *this;
    }

private:
    union {
        char m_var[C_VAR_SPACE_BYTES];
        const void* m_pointer;
    };
    std::string m_string;
    vartype_t m_type;
};
