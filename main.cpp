#include <variant>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>
#include <regex>
#include <charconv>
#include "print.h"

struct JSONObject;

using JSONDict = std::unordered_map<std::string, JSONObject>;
using JSONList = std::vector<JSONObject>;

struct JSONObject {
    std::variant
    < std::nullptr_t  // null
    , bool            // true
    , int             // 42
    , double          // 3.14
    , std::string     // "hello"
    , JSONList        // [42, "hello"]
    , JSONDict        // {"hello": 985, "world": 211}
    > inner;

    void do_print() const {
        printnl(inner);
    }

    template <class T>
    bool is() const {
        return std::holds_alternative<T>(inner);
    }

    template <class T>
    T const &get() const {
        return std::get<T>(inner);
    }

    template <class T>
    T &get() {
        return std::get<T>(inner);
    }
};

template <class T>
std::optional<T> try_parse_num(std::string_view str) {
    T value;
    auto res = std::from_chars(str.data(), str.data() + str.size(), value);
    if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
        return value;
    }
    return std::nullopt;
}

char unescaped_char(char c) {
    switch (c) {
    case 'n': return '\n';
    case 'r': return '\r';
    case '0': return '\0';
    case 't': return '\t';
    case 'v': return '\v';
    case 'f': return '\f';
    case 'b': return '\b';
    case 'a': return '\a';
    default: return c;
    }
}

std::pair<JSONObject, size_t> parse(std::string_view json) {
    if (json.empty()) {
        return {JSONObject{std::nullptr_t{}}, 0};
    } else if (size_t off = json.find_first_not_of(" \n\r\t\v\f\0"); off != 0 && off != json.npos) {
        auto [obj, eaten] = parse(json.substr(off));
        return {std::move(obj), eaten + off};
    } else if ('0' <= json[0] && json[0] <= '9' || json[0] == '+' || json[0] == '-') {
        std::regex num_re{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
        std::cmatch match;
        if (std::regex_search(json.data(), json.data() + json.size(), match, num_re)) {
            std::string str = match.str();
            if (auto num = try_parse_num<int>(str)) {
                return {JSONObject{*num}, str.size()};
            }
            if (auto num = try_parse_num<double>(str)) {
                return {JSONObject{*num}, str.size()};
            }
        }
    } else if (json[0] == 'n') {
    // 检查是否以 "null" 开头，且后面不是标识符字符（字母/数字/下划线）
    if (json.size() >= 4 && json.substr(0, 4) == "null") {
        // 检查第5个字符（如果存在）是否不是 [a-zA-Z0-9_]
        if (json.size() == 4 || !std::isalnum(static_cast<unsigned char>(json[4]))) {
            return {JSONObject{std::nullptr_t{}}, 4};  // 吃掉 4 个字符
        }
    }
    return {JSONObject{std::nullptr_t{}}, 0};  // 解析失败
    
    } else if (json[0] == 't') {
        if (json.size() >= 4 && json.substr(0, 4) == "true") {
            if (json.size() == 4 || !std::isalnum(static_cast<unsigned char>(json[4]))) {
                return {JSONObject{true}, 4};  // bool 值为 true
            }
        }
        return {JSONObject{std::nullptr_t{}}, 0};
    
    } else if (json[0] == 'f') {
        if (json.size() >= 5 && json.substr(0, 5) == "false") {
            if (json.size() == 5 || !std::isalnum(static_cast<unsigned char>(json[5]))) {
                return {JSONObject{false}, 5};  // bool 值为 false，注意长度是 5
            }
        }
        return {JSONObject{std::nullptr_t{}}, 0};
    } else if (json[0] == '"') {
        std::string str;
        enum {
            Raw,
            Escaped,
        } phase = Raw;
        size_t i;
        for (i = 1; i < json.size(); i++) {
            char ch = json[i];
            if (phase == Raw) {
                if (ch == '\\') {
                    phase = Escaped;
                } else if (ch == '"') {
                    i += 1;
                    break;
                } else {
                    str += ch;
                }
            } else if (phase == Escaped) {
                str += unescaped_char(ch);
                phase = Raw;
            }
        }
        return {JSONObject{std::move(str)}, i};
    } else if (json[0] == '[') {
        std::vector<JSONObject> res;
        size_t i;
        for (i = 1; i < json.size();) {
            if (json[i] == ']') {
                i += 1;
                break;
            }
            auto [obj, eaten] = parse(json.substr(i));
            if (eaten == 0) {
                i = 0;
                break;
            }
            res.push_back(std::move(obj));
            i += eaten;
            //在一个数据插入后检查后面的空白，并吃掉
            while(i < json.size() && std::isspace(static_cast<unsigned char>(json[i]))) ++i;
            if (json[i] == ',') {
                i += 1;
            }
        }
        return {JSONObject{std::move(res)}, i};
    } else if (json[0] == '{') {
        std::unordered_map<std::string, JSONObject> res;
        size_t i;
        for (i = 1; i < json.size();) {
            if (json[i] == '}') {
                i += 1;
                break;
            }
            auto [keyobj, keyeaten] = parse(json.substr(i));
            if (keyeaten == 0) {
                i = 0;
                break;
            }
            i += keyeaten;
            if (!std::holds_alternative<std::string>(keyobj.inner)) {
                i = 0;
                break;
            }
            if (json[i] == ':') {
                i += 1;
            }
            std::string key = std::move(std::get<std::string>(keyobj.inner));
            auto [valobj, valeaten] = parse(json.substr(i));
            if (valeaten == 0) {
                i = 0;
                break;
            }
            i += valeaten;
            res.try_emplace(std::move(key), std::move(valobj));

            //在一个数据插入后检查后面的空白，并吃掉
            while(i < json.size() && std::isspace(static_cast<unsigned char>(json[i]))) ++i;

            if (json[i] == ',') {
                i += 1;
            }
        }
        return {JSONObject{std::move(res)}, i};
    }
    return {JSONObject{std::nullptr_t{}}, 0};
}

template <class ...Fs>
struct overloaded : Fs... {
    using Fs::operator()...;
};

template <class ...Fs>
overloaded(Fs...) -> overloaded<Fs...>;

int main() {
    // 测试1：基本字面量（数组中包含所有三种特殊值）
    std::string_view basic = R"JSON([null, true, false, 42])JSON";
    
    // 测试2：深层嵌套（对象嵌套对象，再嵌套数组，包含混合类型）
    std::string_view nested = R"JSON({
        "success": true,
        "error": null,
        "valid": false,
        "nested_obj": {
            "flag": true,
            "empty": null,
            "deep_array": [false, true, null, {"inner_null": null}]
        },
        "mixed_array": [1, false, "text", null, true]
    })JSON";
    
    // 测试3：边界情况（字面量紧跟其他字符，测试是否正确截断）
    std::string_view edge = R"JSON({"a":null,"b":true,"c":false})JSON";

    std::cout << "=== Test 1: Basic literals ===" << std::endl;
    auto [obj1, eaten1] = parse(basic);
    std::cout << "Parse completeness: " << eaten1 << "/" << basic.size() 
              << (eaten1 == basic.size() ? " ✓" : " ✗") << std::endl;
    print(obj1);
    std::cout << std::endl << std::endl;

    std::cout << "=== Test 2: Deeply nested structures ===" << std::endl;
    auto [obj2, eaten2] = parse(nested);
    std::cout << "Parse completeness: " << eaten2 << "/" << nested.size()
              << (eaten2 == nested.size() ? " ✓" : " ✗") << std::endl;
    print(obj2);
    std::cout << std::endl << std::endl;

    // 关键验证：使用 visit 检查类型是否正确识别
    std::cout << "=== Type verification (root object) ===" << std::endl;
    std::visit(
        overloaded{
            [&] (std::nullptr_t) {
                print("Root is: null");
            },
            [&] (bool val) {
                print("Root is: bool =", val ? "true" : "false");
            },
            [&] (int val) {
                print("Root is: int =", val);
            },
            [&] (double val) {
                print("Root is: double =", val);
            },
            [&] (const std::string& val) {
                print("Root is: string =", val);
            },
            [&] (const auto& container) {
                // 处理 vector<JSONObject> (数组) 或 unordered_map (对象)
                print("Root is: container with", container.size(), "elements");
            },
        },
        obj2.inner  // 验证嵌套对象的根类型（应该是 object/map）
    );

    // 递归验证：手动深入检查嵌套值
    std::cout << "\n=== Deep value verification ===" << std::endl;
    if (std::holds_alternative<JSONDict>(obj2.inner)) {
        const auto& root_map = std::get<JSONDict>(obj2.inner);
        
        // 检查 "success": true
        auto it = root_map.find("success");
        if (it != root_map.end() && std::holds_alternative<bool>(it->second.inner)) {
            std::cout << "success: " 
                      << (std::get<bool>(it->second.inner) ? "true" : "false") 
                      << " ✓" << std::endl;
        }
        
        // 检查 "error": null
        it = root_map.find("error");
        if (it != root_map.end() && std::holds_alternative<std::nullptr_t>(it->second.inner)) {
            std::cout << "error: null ✓" << std::endl;
        }
        
        // 检查 "valid": false
        it = root_map.find("valid");
        if (it != root_map.end() && std::holds_alternative<bool>(it->second.inner)) {
            std::cout << "valid: " 
                      << (std::get<bool>(it->second.inner) ? "true" : "false") 
                      << " ✓" << std::endl;
        }
    }

    return 0;
}