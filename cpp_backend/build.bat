if (Test-Path build) { Remove-Item -Recurse -Force build }
mkdir build
cd build

cmake .. -A x64 -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake -DPython_ROOT_DIR="C:/Users/natha/AppData/Local/Programs/Python/Python312" -DPython_EXECUTABLE="C:/Users/natha/AppData/Local/Programs/Python/Python312/python.exe"

cmake --build . --config Release
