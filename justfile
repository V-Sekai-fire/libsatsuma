# Define variables
export build_dir := "build"

# Default build task
build:
    @echo "Creating build directory if it doesn't exist..."
    mkdir -p $build_dir
    @echo "Running CMake..."
    cmake -S . -B $build_dir
    @echo "Building the project..."
    cmake --build $build_dir

# Run the built executable
run: build
    @echo "Running the application..."
    ./$build_dir/YourExecutableName

# Clean the build directory
clean:
    @echo "Cleaning up..."
    rm -rf $build_dir

# Help command to display available commands
help:
    @echo "Available commands:"
    @echo "  build: Build the project"
    @echo "  run: Build and run the project"
    @echo "  clean: Remove build artifacts"
