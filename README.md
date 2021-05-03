# Lynx Renderer

## Build Steps:
1. install cmake
2. go to \build
3. make sure Visual Studio version matches setup_win64.bat
4. shift-right click and open powershell in \build
5. run '.\setup_win64.bat ga1-core'
6. the project should build and create a Visual Studio solution

### Due to cmake struggle, assimp need to be linked manually:
1. After building the project, go to ga project properties
2. Go to Linker/Input/additional dependencies
3. Add "assimp-vc142-mt.lib"

## Run Steps:
1. open the solution using Visual Studio 
2. set 'ga' to default startup project (right-click on the solution panel)
3. run via Visual Studio

## Controls:
- WASD: move camera
- Arrow keys: rotate camera
- Z: toggle selected entity
- F: focus on selected entity 
- L: show framework
- K: show lit models