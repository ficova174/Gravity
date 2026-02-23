# Changelog

All notable changes to this project will be documented in this file.  
This project adheres to [Semantic Versioning](https://semver.org/).

---

## [0.7.0] - 2026-02-23
### Added
- Buttons for adding/removing columns/rows of the map

### Changed
- Particles are not initialised by checking for collisions, they are spawned and collision is treated in the next frame
- Map class changed a bit, now we are giving nbColumns and nbRows in the constructor and we can change the map size on the fly
- Map is now a grid with borders
- Map class has a rectangle for the viewport and one for the particles, consequently the other classes changed according to the new viewports
- Getter added to the viewport class in case the map gets smaller than the viewport when changin the size

---

## [0.6.1] - 2026-02-20
### Added
- Debugging folder with a timer utility in order to measure time for some part of the code to execute

### Changed
- Modified the map class, now nbColumns, nbRows and size are given in the simulation and map constructor.
- Map texture initialisation doesn't use surfaces anymore, it is clearer and faster (0.86 sec -> 0.26 sec)

### Fixed
- New exception if a particle is bigger than the map

---

## [0.6.0] - 2026-02-20
### Added
- Added Dear ImGui files
- Added a Dear ImGui window displaying the fps count, enabling the user to set the number of particles and displaying the total kinetic energy

### Changed
- Now viewport as big as possible to get an overall view of the map
- Viewport::setCoordinates() deleted
- Added Readme and License of Eigen

---

## [0.5.1] - 2026-02-19
### Added
- Checking if particles fit in the map during initialisation, if not throws error

### Fixed
- Buggy physics collisions
- Overlaping/jiggling

---

## [0.5.0] - 2026-02-19
### Added
- Multiple particle simulation with BUGGY collisions
- Documented my code
- Started versioning

### Changed
- We are now using Eigen for doing math in c++

---

## [0.4.0] - 2026-02-18
### Added
- Introducing Multiple particle logic

### Changed
- Refactored the code structure to make it clearer

---

## [0.3.0] - 2026-02-18
### Added
- Particle that moves around and bounces of the walls
- Particle size depends on its mass

---

## [0.2.0] - 2026-02-17
### Changed
- Map is now created "by hand" (chessboard) instead of loading a png image

---

## [0.1.0] - 2026-02-17
### Added
- Scraped project using SDL3 with basic functionalities : Map, viewport, zooming, moving and basic game loop
