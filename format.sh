find nemu abstract-machine nanos-lite \( -name "*.c" -o -name "*.h" \) -not -path "nemu/tools/spike-diff/repo/*" -not -path "nemu/include/generated/*" -prune -exec clang-format-18 -style=file -i {} \;