const fs = require('fs');

// mox

let file = ['#linkc fn main(): i32 { return sum199999(0); }\n fn sum0(x: i32): i32 { return x + 1; }'];

for (let i = 1; i < 200000; ++i) {
    file.push(`fn sum${i}(x: i32): i32 {
        return sum${i-1}(x) + 1;
    }`);
}

fs.writeFileSync("./200k.mox", file.join('\n'));

// clang

file = [`int sum199999(int); int main() { return sum199999(0); } int sum0(int x) { return x + 1; }`];

for (let i = 1; i < 200000; ++i) {
    file.push(`int sum${i}(int x) {
        return sum${i-1}(x) + 1;
    }`);
}

fs.writeFileSync("./200k.c", file.join('\n'));

// rust

file = [`pub fn main() {
    std::process::exit(sum199999(0));
} \n fn sum0(x: i32) -> i32 { x + 1 }`];

for (let i = 1; i < 200000; ++i) {
    file.push(`fn sum${i}(x: i32) -> i32 {
        sum${i-1}(x) + 1
    }`);
}

fs.writeFileSync("./200k.rs", file.join('\n'));

// odin

file = [`package main\n\nimport "core:os"\n\nmain :: proc () {
    os.exit(int(sum199999(0)));
} \n sum0 :: proc (x: i32) -> i32 { return x + 1; }`];

for (let i = 1; i < 200000; ++i) {
    file.push(`sum${i} :: proc (x: i32) -> i32 {
        return sum${i-1}(x) + 1;
    }`);
}

fs.writeFileSync("./200k.odin", file.join('\n'));
