# Automatic Guard Placement System Plugin for Unreal Engine 5
This plugin provides an automatic guard placement system for Unreal Engine 5. Diffrent algorithms can be used / implemented to place guards in a level, based on the geometry of the level and the desired coverage.

## Features
- Automatic guard placement based on level navmesh
- Extendable guard placement algorithms

## How to use

![AGPS Screenshot](Resources/CapturePluginUsage.png)
1. Select a navmesh in your level.
2. Select a guard placement algorithm from the dropdown menu.
3. Click the "Place Guards" button to automatically place guards in the level based on the selected algorithm. Or use the "Preview Guards" button to see a preview of the guard placements before placing them.

## Algorithms
The plugin currently includes the following guard placement algorithms:
- **Tricoloration**: This algorithm uses a tricoloration approach to place guards in a way that maximizes coverage while minimizing the number of guards needed. It assigns one of three colors to each guard, ensuring that no two guards of the same color are adjacent to each other.
