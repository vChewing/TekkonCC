find . -regex '.*\.\(hh\)' -exec clang-format -style=file -i {} \;
find . -regex '.*\.\(cc\)' -exec clang-format -style=file -i {} \;
find . -regex '.*\.\(mm\)' -exec clang-format -style=file -i {} \;
