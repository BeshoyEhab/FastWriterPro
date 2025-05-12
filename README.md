# FastWriterPro

A modern, intelligent text editor with powerful autocomplete functionality, built with Qt.

![FastWriterPro](https://via.placeholder.com/800x450?text=FastWriterPro+Screenshot)

## Features

- **Smart Autocomplete**: Suggestions appear as you type, adapting to your writing style
- **Custom Trie Data Structure**: Efficient word storage and retrieval
- **Frequency-Based Suggestions**: Words you use more often appear first
- **BFS/DFS Search Options**: Customize how suggestions are prioritized
- **Modern UI**: Clean, responsive dark theme interface
- **Custom Dictionary Management**: Add or remove words from your personal dictionary
- **Smooth Animations**: Polished user experience with animated suggestion display

## Getting Started

### Prerequisites

- Qt 6.0 or later
- CMake 3.16 or later
- C++17 compatible compiler

### Installation

#### From Source

1. Clone the repository:
```bash
git clone https://github.com/yourusername/FastWriterPro.git
cd FastWriterPro
```

2. Create and navigate to a build directory:
```bash
mkdir build && cd build
```

3. Configure the project with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
make
```

5. Run the application:
```bash
./FastWriterPro
```

#### Pre-built Binaries

Download the latest release for your platform from the releases page.

## Usage

1. Start typing in the input field
2. As you type, suggestion buttons will appear above the input field
3. Select a suggestion using Tab (forward) and Shift+Tab (backward)
4. Press Enter to accept the selected suggestion
5. Press Space to add the current word to the dictionary if it's new

### Customization

Click on Settings -> Preferences to:
- Change search method (BFS/DFS)
- Adjust maximum number of suggestions shown
- Toggle frequency-based sorting
- Add or remove words from the dictionary

## How It Works

FastWriterPro uses a custom Trie data structure to efficiently store and retrieve words. As you type, the application searches the Trie for matching prefixes and presents suggestions based on your settings:

- **BFS (Breadth-First Search)**: Prioritizes shorter completions
- **DFS (Depth-First Search)**: Shows suggestions in alphabetical order
- **Frequency Sorting**: When enabled, ranks suggestions by usage frequency

## Project Structure

```
.
├── src/                  # Source files
│   ├── autocompleteapp.cpp  # Main application window
│   ├── inputfield.cpp    # Custom input field widget
│   ├── hoverablebutton.cpp  # Interactive button component
│   ├── trie.cpp          # Trie data structure implementation
│   ├── trienode.cpp      # Trie node implementation
│   ├── settingsdialog.cpp  # Preferences dialog
│   └── main.cpp          # Application entry point
├── headers/              # Header files
├── assets/               # Resources
│   ├── Style.css         # Application styling
│   └── words_dictionary.json  # Default dictionary
├── data_model/           # Data handling
│   ├── model.cpp         # Dictionary file operations
│   └── model.h           # Model header
└── CMakeLists.txt        # CMake build configuration
```

## Contributing

Contributions are welcome! Please feel free to submit a pull request.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Qt framework for providing the foundation for the UI
- Team GEN_005 for their contributions to the project
