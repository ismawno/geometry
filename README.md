# shapes-2D

shapes-2D is a library for working with convex polygons in 2D space. It provides a variety of operations for building, manipulating, and analyzing polygons.

## Features

- Convex polygon implementation
- Operations for translating, checking convexity, rotating, sorting vertices, computing center of mass, inertia, area, Minkowski sum and difference, and finding the closest edge to a point
- AABB implementation for broad-phase collision detection
- Supports saving and loading polygon state to/from an INI file using ini-parser

## Dependencies

shapes-2D depends on two other projects, all created by the same author:

- [glm](https://github.com/g-truc/glm)
- [debug-tools](https://github.com/ismawno/debug-tools)
- [ini-parser](https://github.com/ismawno/ini-parser)
- [profile-tools](https://github.com/ismawno/profile-tools)

The [fetch_dependencies.py](https://github.com/ismawno/shapes-2D/scripts/fetch_dependencies.py) script will automatically add all the dependencies as git submodules, provided that the user has already created their own repository and included the current project as a git submodule (or at least downloaded it into the repository). To ensure all runs smoothly once the script has been executed, do not rename the folders containing the various dependencies. All external dependencies, those not created by the same author, will be added as submodules within the [vendor](https://github.com/ismawno/shapes-2D/vendor) folder.

## Building and Usage

1. Ensure you have `premake5` and `make` installed on your system. `premake5` is used to generate the build files, and `make` is used to compile the project.
2. Create your own repository and include the current project as a git submodule (or at least download it into the repository).
3. Run the [fetch_dependencies.py](https://github.com/ismawno/shapes-2D/scripts/fetch_dependencies.py) script located in the [scripts](https://github.com/ismawno/shapes-2D/scripts) folder to automatically add all the dependencies as git submodules.
4. Create an entry point project with a `premake5` file, where the `main.cpp` will be located. Link all libraries and specify the kind of the executable as `ConsoleApp`. Don't forget to specify the different configurations for the project.
5. Create a `premake5` file at the root of the repository describing the `premake` workspace and including all dependency projects.
6. Build the entire project by running the `make` command in your terminal. You can specify the configuration by using `make config=the_configuration`.
7. To use shapes-2D, simply include the [polygon.hpp](https://github.com/ismawno/shapes-2D/include/geo/polygon.hpp) ot the [aabb2D.hpp](https://github.com/ismawno/shapes-2D/include/geo/aabb2D.hpp) header in your project.

For more information on how to use shapes-2D, please refer to the documentation.

## License

shapes-2D is licensed under the MIT License. See LICENSE for more information.
