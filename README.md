# ⚔️ IRON DUEL // BALLISTIC_ENGINE
> **Project Type:** High-Performance 2D Physics Simulator  
> **Core Tech:** C++17, SDL2, OpenGL

### 📂 SYSTEM_OVERVIEW
Iron Duel is a reimagining of the classic "Scorched Earth" artillery mechanic. The engine focuses on low-level optimization to handle high-density particle effects and procedural terrain destruction.

### 🛠 TECHNICAL_SOLUTIONS
- **Destructible Terrain:** Implemented a **Bitmask Coordinate System**. Instead of re-calculating the entire map mesh, the engine modifies bit-arrays for pixel-perfect destruction at $O(1)$ complexity.
- **Ballistics Solver:** Custom differential equations to account for drag coefficients, wind vectors, and gravitational constants.
- **Rendering:** Batch-rendering of projectiles to minimize draw calls.

## 🛠 Installation & Setup

### Prerequisites
This project requires a C++ compiler (GCC/Clang) and the **SDL2** development library.

#### 🐧 Arch Linux
```bash
just exec the program its compiled for linux though 
