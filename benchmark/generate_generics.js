const fs = require('fs');

const N = 25000;
const TYPES = ['i8', 'i16', 'i32', 'i64', 'u8', 'u16', 'u32', 'u64'];

const CPP_TYPES = {
    i8: 'int8_t', i16: 'int16_t', i32: 'int32_t', i64: 'int64_t',
    u8: 'uint8_t', u16: 'uint16_t', u32: 'uint32_t', u64: 'uint64_t',
};

function prev(i, lang_call) {
    return i === 0 ? '' : lang_call(i - 1) + ' + ';
}

// mox

let file = [];

for (let i = 0; i < N; ++i) {
    const locals = TYPES.map(t => `    v_${t}:= Box${i}(${t}).{ 1; 2; };`).join('\n');
    const calls = TYPES.map(t => `get${i}(v_${t}).cast(i32)`).join(' + ');
    file.push(`struct Box${i}($T: __type_ptr) { a: $T; b: $T; };
fn get${i}(x: Box${i}($T)): $T { return x.a + x.b; }
fn use${i}(): i32 {
${locals}
    return ${prev(i, j => `use${j}()`)}${calls};
}`);
}

file.push(`#linkc fn main(): i32 { return use${N - 1}(); }`);

fs.writeFileSync("./generics.mox", file.join('\n'));

// clang++

file = ['#include <stdint.h>'];

for (let i = 0; i < N; ++i) {
    const locals = TYPES.map(t => `    Box${i}<${CPP_TYPES[t]}> v_${t}{1, 2};`).join('\n');
    const calls = TYPES.map(t => `(int)get${i}(v_${t})`).join(' + ');
    file.push(`template<typename T> struct Box${i} { T a; T b; };
template<typename T> T get${i}(Box${i}<T> x) { return x.a + x.b; }
int use${i}() {
${locals}
    return ${prev(i, j => `use${j}()`)}${calls};
}`);
}

file.push(`int main() { return use${N - 1}(); }`);

fs.writeFileSync("./generics.cpp", file.join('\n'));

// rust

file = [];

for (let i = 0; i < N; ++i) {
    const locals = TYPES.map(t => `    let v_${t} = Box${i}::<${t}> { a: 1, b: 2 };`).join('\n');
    const calls = TYPES.map(t => `get${i}(v_${t}) as i32`).join(' + ');
    file.push(`struct Box${i}<T> { a: T, b: T }
fn get${i}<T: std::ops::Add<Output = T>>(x: Box${i}<T>) -> T { x.a + x.b }
fn use${i}() -> i32 {
${locals}
    ${prev(i, j => `use${j}()`)}${calls}
}`);
}

file.push(`pub fn main() {
    std::process::exit(use${N - 1}());
}`);

fs.writeFileSync("./generics.rs", file.join('\n'));

// odin

file = [`package main\n\nimport "core:os"`];

for (let i = 0; i < N; ++i) {
    const locals = TYPES.map(t => `    v_${t} := Box${i}(${t}){1, 2};`).join('\n');
    const calls = TYPES.map(t => `i32(get${i}(v_${t}))`).join(' + ');
    file.push(`Box${i} :: struct($T: typeid) { a: T, b: T }
get${i} :: proc(x: Box${i}($T)) -> T { return x.a + x.b; }
use${i} :: proc() -> i32 {
${locals}
    return ${prev(i, j => `use${j}()`)}${calls};
}`);
}

file.push(`main :: proc () {
    os.exit(int(use${N - 1}()));
}`);

fs.writeFileSync("./generics.odin", file.join('\n'));
