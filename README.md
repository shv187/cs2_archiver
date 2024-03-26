# CS2 Archiver
It's a small tool I wrote to backup CS2 files easly.

# Description
It creates the following structure:
```
.
├── binaries
│   ├── <patch_version> 
│      ├── client
│         ├── <files>
│      ├── engine
│         ├── <files>
└──
```

# Notes
It's ❗Windows ONLY❗ at the moment.\
It might change in the future if requested by many users in [issues](https://github.com/shv187/cs2_archiver/issues) or if I'll need it on some other OS.

# Requirements
* [CMake](https://cmake.org/)
* MSVC
* C++23

# Usage
```
git clone https://github.com/shv187/cs2_archiver.git
```
```
cd cs2_archiver
```
```
cmake -S . -B build && cmake --build build --config Release && start build\Release
```
```
<run the executable>
```
Or simply download from [Releases](https://github.com/shv187/cs2_archiver/releases)

# Help 
Feel free to open issues :)
