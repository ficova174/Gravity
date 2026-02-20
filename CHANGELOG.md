# Changelog

All notable changes to this project will be documented in this file.  
This project adheres to [Semantic Versioning](https://semver.org/).

---

## [Unreleased]
### Added
- Added Dear ImGui files
- Added working Dear ImGui demo window

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
