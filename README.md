
---

C++/Arduino Project to control a WS2812B Strip using a keyboard connected to your PC (WIP)

---

# Installing dependencies

- sudo apt install xinput	
- sudo apt install libx11-dev
- sudo apt-get install libxi-dev
- https://github.com/gbmhunter/CppLinuxSerial

# Compiling 

- export LIBRARY_PATH=/usr/local/lib/
- gcc ./Main.cpp -lstdc++ -lX11 -lXext -lXi -lCppLinuxSerial

# Running 

- Upload the arduino sketch to your board
- Find your device id by running xinput in a terminal
- ./a.out <device id>

