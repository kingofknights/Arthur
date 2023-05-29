@echo "Activating EMSDK"
cd C:\Apps\emsdk
emsdk activate latest

@echo "Going to Application"

cd C:\Users\viklod\Music\Projects\Experimental\tradingapp\build\
emcmake cmake -G "Visual Studio 17 2022" -A x64 ..

@echo "Building Project"
emcmake cmake --build . --target ALL_BUILD

# Ninja
# cmake --build .
