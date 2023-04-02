# shapes-2D

shapes-2D is a library for working with convex polygons in 2D space. It provides a variety of operations for building, manipulating, and analyzing polygons.

## Features

- Convex polygon implementation
- Operations for translating, checking convexity, rotating, sorting vertices, computing center of mass, inertia, area, Minkowski sum and difference, and finding the closest edge to a point
- AABB implementation for broad-phase collision detection
- Supports saving and loading polygon state to/from an INI file using ini-parser

## Dependencies

shapes-2D depends on two other projects, all created by the same author:

- [vec-2D](https://github.com/Ismael99Bueno/vec-2D): a 2D vector implementation with usual operations
- [debug-tools](https://github.com/Ismael99Bueno/debug-tools): a set of tools for debugging poly-physx simulations
- [ini-parser](https://github.com/Ismael99Bueno/ini-parser): a simple INI file parser that allows for reading and writing the state of a polygon to and from a file

## Usage

To use shapes-2D, simply include the polygon.hpp ot the aabb2D.hpp header in your project.

shapes-2D is built using premake5 as a static library. To use it, you must create a premake5 workspace that includes the shapes-2D library and the ini-parser library. You can then build the workspace with premake5 to create an executable.

It is advisable to include shapes-2D in a user-made repository as a git submodule. This allows you to keep the shapes-2D code separate from your own code, making it easier to manage dependencies and track changes.

For more information on how to use shapes-2D, please refer to the documentation.

## License

shapes-2D is licensed under the MIT License. See LICENSE for more information.
