#!/bin/bash

# Check for GTK3 development libraries
if ! pkg-config --exists gtk+-3.0; then
    echo "Error: GTK3 development libraries not found!"
    echo "  On Ubuntu/Debian: sudo apt install libgtk-3-dev"
    echo "  On Fedora: sudo dnf install gtk3-devel"
    echo "  On macOS with Homebrew: brew install gtk+3"
    exit 1
fi

# Check for Graphviz
if ! command -v dot &> /dev/null; then
    echo "Error: Graphviz not found!"
    echo "  On Ubuntu/Debian: sudo apt install graphviz"
    echo "  On Fedora: sudo dnf install graphviz"
    echo "  On macOS with Homebrew: brew install graphviz"
    exit 1
fi

# Build the compiler components and GUI
echo "Building TINY Compiler..."
make clean
make scanner parser guii

# Check for build errors
if [ $? -ne 0 ]; then
    echo "Build failed! Please check the error messages above."
    exit 1
fi

# Create sample TINY program if none exists
if [ ! -f "input.tiny" ]; then
    echo "Creating sample TINY program (input.tiny)..."
    cat > input.tiny << EOF
{ Sample TINY program }
read x;
if x < 0 then
  read x;
repeat
  x := x - 1;
  write x;
until x = 0;
end
EOF
fi

# Run the GUI
echo "Starting TINY Compiler GUI..."
./gui