# Raycaster

A simple 2D raycasting project implemented in C++ using the Qt framework. This project demonstrates basic raycasting
algorithms by computing the intersection of rays with polygons to simulate a light area effect. It features interactive
modes allowing you to switch between light simulation and polygon drawing.

## Overview

This project consists of several classes that manage the behavior of rays, polygons, and the overall graphical user
interface. The central components are:

- **Ray**: Represents a ray defined by a starting point, an ending point, and an angle. It supports rotation operations.
- **Polygon**: Represents a polygon composed of vertices. It includes methods for adding vertices and computing
  intersections with rays.
- **Controller**: Manages application modes and implements the core raycasting algorithm, polygon updates, and light
  area creation.
- **MainWindow & DrawingWidget**: Handle the UI components and user interactions (e.g., mouse movements and clicks) to
  render and update the scene in real time.

## Requirements

- **Qt Framework**: Version 5.x or 6.x (Ensure the Qt Widgets module is available).
- **C++ Compiler**: A C++17-compliant compiler (or later).
- **Build Tools**: You can use one of the following build systems:
    - **Bazel** (with the provided BUILD file)

## Build Instructions

### Using Bazel

The project includes a Bazel `BUILD` file using the [rules_qt6](https://github.com/pkasila/rules_qt6) for Qt
integration. To build the project with Bazel:

1. **Install Bazel** if you haven't already. Follow the instructions on the [Bazel website](https://bazel.build/).

2. **Build the project** using:
    ```bash
    bazel build //labs/raycaster
    ```

3. **Run the executable**:
    ```bash
    bazel run //labs/raycaster
    ```

Your Bazel `BUILD` file defines libraries for each module (e.g., `main_window`, `ray`, `polygon`, `controller`, and
`drawing_widget`) and the final binary target `raycaster`.

## Usage

- **Light Mode** (default):  
  Moving the mouse repositions the light source. Rays are cast from the light source to the polygon vertices, and
  intersections are computed in real time to simulate shadows/light areas.

- **Polygon Mode**:  
  Left-click to start drawing a new polygon or to add vertices to the current polygon. Right-click to finalize the
  current polygon.

- **Mode Selection**:  
  Toggle between Light and Polygon modes using the drop-down menu located in the top-left corner of the application
  window.

- **Performance Indicator**:  
  The application displays the current frame rate (FPS) in the top-right corner of the window.

## Using OpenGL (Optional)

If you would like to leverage OpenGL for potentially higher FPS and more advanced rendering features, you can revert to
a previous commit that contains the OpenGL setup. 
   ```bash
    git checkout 51e8a97 
   ```