# Qt Autocomplete Application

A modern Qt-based application that provides autocomplete functionality with a custom user interface. This project is developed by team GEN_005.

## Features

- Custom input field with autocomplete capabilities
- Hoverable button component with visual feedback
- Modern and responsive UI design
- CMake-based build system

## Project Structure

```
.
├── main.cpp               # Application entry point
├── src/                   # Source files
│   ├── autocompleteapp.cpp # Main application implementation
│   ├── inputfield.cpp     # Custom input field implementation
│   └── hoverablebutton.cpp # Custom button implementation
├── headers/              # Header files
│   ├── autocompleteapp.h # Main application header
│   ├── inputfield.h      # Custom input field header
│   └── hoverablebutton.h # Custom button header
├── CMakeLists.txt        # CMake build configuration
└── README.md             # Project documentation
```

## Prerequisites

- Qt 6 or later
- CMake 3.16 or later
- C++17 compatible compiler

## Building the Project

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure the project with CMake:
```bash
cmake ..
```

3. Build the project:
```bash
make
```

## Running the Application

After building, you can run the application from the build directory:
```bash
./FastWriterPro
```

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
