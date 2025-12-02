Iron Duel

Iron Duel is a hobby artillery game inspired by the classic Scorched Earth formula. It focuses on precision, simple controls, and fully destructible terrain. The goal is straightforward: adjust angle, power, and timing to eliminate opponents across varied arenas.

The project is written in C++ and built primarily for Linux. It aims to stay lightweight, clear, and easy to modify, making it a useful sandbox for learning game development fundamentals, system design, and simulation logic.

Features

• Turn-based artillery combat with adjustable angle and shot power
• Destructible terrain that changes the battlefield with every hit
• Multiple maps, each offering different tactical challenges
• Simple physics model focused on readability and predictable behavior
• Clean C++ codebase designed for experimentation and extension
• No external engines — just standard libraries and your own logic

Current Mechanics

• Players take turns firing projectiles.
• Explosions dynamically modify the terrain mesh or heightmap.
• Projectile arcs follow a straightforward ballistic curve.
• Damage depends on distance to impact.
• Map layouts load from lightweight data files.

Dependencies:
• A C++ compiler supporting C++17 or newer (GCC/Clang)
• Make
• SDL 1.2

make

Controls

Arrow keys – Adjust cannon angle
up / down - Change shot power
Space – Fire
R – Reset current match
Esc – Quit

Roadmap

Some ideas for future expansions:

• Improved terrain deformation
• Additional weapons or projectile types
• Wind or environmental effects
• More advanced physics
• AI opponent
• Basic multiplayer (local or networked)
• Optional UI polish or sound

This is a hobby project, so features evolve gradually as skills grow.

Contributing

This project is primarily for personal learning. Pull requests are welcome, but the focus is on clarity and experimentation rather than large-engineered features.
