# BabyJSON - 学习版 / Learning Edition

基于 [babyjson-demo] 教育项目的 C++17 JSON 解析器实现，用于深入理解字符串与数字字面量解析、现代 C++ 特性 `std::variant` 与 `std::optional` 的优雅运用。

A C++17 JSON parser implementation based on the [babyjson-demo] educational project, designed for in-depth understanding of string/numeric literal parsing and elegant usage of modern C++ features `std::variant` and `std::optional`.

---

## 致谢 / Acknowledgments

本项目源自 [babyjson-demo] 教学项目，感谢原作者提供的优秀教程与作业设计。

- **原项目链接**: https://github.com/archibate/babyjson-demo
- **Bilibili 视频教程**: https://www.bilibili.com/video/BV1pa4y1g7v6

This project is derived from the [babyjson-demo] educational project. Sincere thanks to the original author for the excellent tutorial and homework design.

- **Original Project**: https://github.com/archibate/babyjson-demo
- **Video Tutorial**: https://www.bilibili.com/video/BV1pa4y1g7v6

---

## 特性 / Features

- ✅ **基础解析 / Basic Parsing**: 字符串、数字、对象、数组
- ✅ **字面量支持 / Literal Support**: `null`, `false`, `true` 的完整解析
- ✅ **引号兼容 / Quote Flexibility**: 同时支持单引号 `' '` 和双引号 `" "` 字符串
- ✅ **转义序列 / Escape Sequences**: 支持十六进制转义 `\x0D`
- ✅ **Unicode 支持 / Unicode Support**: UCS-2 (`\u000D`) 与 UCS-4 (`\U0000000D`) 转义序列，自动编码为 UTF-8
- ✅ **Dumper 实现 / Dumper**: 逆向解析器，支持格式化输出
- ✅ **裸键名支持 / Unquoted Keys**: JSONDict 允许无引号键名
- ✅ **YAML 扩展 / YAML Extension**: 完整 YAML 1.2 解析与序列化支持

---

## 作业进度 / Homework Progress

### 基础作业 / Basic Homework
- [] 1. 实现三种特殊字面量 `null`, `false`, `true` 的解析，并测试递归情况
- [] 2. 支持单引号字符串字面量（`'string'` 与 `"string"` 等价）
- [] 3. 支持十六进制字符转义序列 `\x0D`（需增加 Hex1, Hex2 状态）

### 进阶挑战 / Advanced Challenges
- [] 1. 支持 UCS-2 字符转义序列 `\u000D`，编码为 UTF-8
- [] 2. 支持 UCS-4 字符转义序列 `\U0000000D`，编码为 UTF-8  
- [] 3. 支持任意 JSONObject 作为 JSONDict 的键（需实现 hash 与 equal_to 特性）
- [] 4. 实现 JSON Dumper（解析器的逆向操作）
- [] 5. 为 `dump()` 添加可选参数 `isPretty`，支持格式化缩进输出
- [] 6. 支持 JSONDict 键名可选地不加引号（`{"hello": "world"}` 与 `{hello: "world"}` 等价）
- [] 7. 将此 JSON 解析器/导出器扩展为完整的 YAML 解析器/导出器（YAML 是 JSON 的超集）

---

## 构建与运行 / Build & Run

```bash
cmake -B build
cmake --build build
./build/main
```

---

## 技术亮点 / Technical Highlights


---

## 许可证 / License


---

## 相关链接 / Related Links
