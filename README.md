git clone --recurse-submodules https://github.com/HermitPal/C-Webmake.git

cd C-Webmake

cmake -S . -B build

cmake --build build --config Release

mv build/Release/my_md4c_app.exe ./
