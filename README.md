# Double Pendulum Simulator

A real-time double pendulum physics simulator with a Tron-inspired visual aesthetic. Built with C, SDL2, and custom RK4 integration for accurate chaotic motion simulation.

## Features

- Real-time double pendulum simulation using 4th-order Runge-Kutta (RK4) integration
- Neon trail rendering with additive blending for visual effect
- Energy conservation display and tracking
- Configurable simulation speed
- Interactive controls for pausing, resetting, and randomizing initial conditions
- Grid overlay for reference
- HUD displaying system parameters and total energy

## Physics

The simulator implements the double pendulum equations of motion derived from Lagrangian mechanics. The system exhibits chaotic behavior, where small changes in initial conditions lead to drastically different trajectories over time.

The simulation uses:
- RK4 integration for numerical stability
- Optional damping factor to reduce numerical drift
- Energy calculation (kinetic + potential) for validation

## Dependencies

- SDL2
- SDL2_ttf
- Standard C library (math.h, stdlib.h, stdio.h)

### Installation (Arch Linux)
```bash
sudo pacman -S sdl2 sdl2_ttf
```

### Installation (Debian/Ubuntu)

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

### Installation (Fedora)

```bash
sudo dnf install SDL2-devel SDL2_ttf-devel
```

### Installation (macOS)

```bash
brew install sdl2 sdl2_ttf
```

## Building

The project uses a Makefile for compilation:

```bash
make
```

This will compile the source files and create the executable `bin/double_pendulum_tron`.

To clean build artifacts:

```bash
make clean
```

## Running

After building, run the simulator:

```bash
./bin/double_pendulum_tron
```

Or use make to build and run:

```bash
make run
```

## Controls

| Key | Action |
|-----|--------|
| SPACE | Pause/unpause simulation |
| R | Reset to initial conditions and clear trail |
| X | Randomize initial angles and clear trail |
| Z | Decrease simulation speed (multiply by 0.6) |
| C | Increase simulation speed (multiply by 1.6) |
| T | Toggle trail rendering on/off |
| G | Toggle grid overlay on/off |
| H | Toggle HUD display on/off |
| ESC | Exit application |

## File Structure

```
double-pendulum
├── assets
│   └── JetBrainsMonoNerdFontMono-Regular.ttf
├── bin
│   └── double_pendulum_tron
├── Makefile
├── README.md
└── src
    ├── main.c          # Main loop and event handling
    ├── physics.c       # Pendulum dynamics and RK4 integration
    ├── physics.h
    ├── render.c        # SDL2 rendering and visualization
    ├── render.h
    ├── trails.c        # Trail buffer management
    └── trails.h
```

## Customization

### Physics Parameters

Edit `physics.c` in the `pendulum_init()` function to modify:

- `m1`, `m2`: Masses of the pendulum bobs (default: 1.0 each)
- `l1`, `l2`: Lengths of the pendulum rods (default: 170.0 pixels each)
- `theta1`, `theta2`: Initial angles in radians (default: 0.6π and 0.4π)
- `damping`: Numerical damping factor (default: 0.0)

### Visual Settings

Edit `render.c` to modify:

- Color scheme (tron_blue, tron_accent, bg_dark)
- Grid spacing (default: 40 pixels)
- Trail length (set in main.c trail_create, default: 4000 points)
- Glow intensity and radius

### Simulation Settings

Edit `main.c` to modify:

- Window size (default: 1280x720)
- Physics timestep `dt` (default: 0.008 seconds)
- Initial simulation speed (default: 2.5x)

## Technical Details

### Integration Method

The simulator uses the 4th-order Runge-Kutta (RK4) method for time integration. This provides a good balance between accuracy and computational efficiency for the chaotic double pendulum system.

### Trail Rendering

Trails are rendered using a circular buffer that stores the position history of the second pendulum bob. The trail uses additive blending with alpha gradients to create a glowing neon effect.

### Energy Conservation

The HUD displays the total mechanical energy (kinetic + potential). In an ideal simulation, this value should remain constant. Small variations indicate numerical drift, which can be reduced by:

- Decreasing the timestep `dt`
- Adding small damping (trade-off: physical accuracy)
- Using double precision (already implemented)

## Known Limitations

- No collision detection between pendulum segments
- Trails are rendered as 1-pixel lines regardless of width parameter
- Font fallback is limited to common system paths
- No configuration file support (requires recompilation for parameter changes)

## License

This project is provided as-is for educational and research purposes.

## Acknowledgments

Inspired by the chaotic beauty of double pendulum systems and Tron's visual aesthetic.
