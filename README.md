# Top-Down-Shooter
A simple top-down space shooter built with C++ and SDL3.


## Directory Structure

```
Top-Down-Shooter/
├── include/                 # Header files
│   ├── engine/             # ECS engine headers
│   │   ├── Types.h         # Core ECS types (Entity, Component, Signature)
│   │   ├── EntityManager.h # Entity management system
│   │   ├── ComponentStorage.h # Component storage system
│   │   ├── ComponentManager.h # Component management
│   │   ├── System.h        # System base class
│   │   ├── SystemManager.h # System management
│   │   └── Registry.h      # ECS registry/manager
│   │
│   └── game/               # Game-specific headers
│       ├── components.h    # All components in one header
│       ├── systems/        # System headers (future)
│       ├── entities/       # Entity blueprints (future)
│       └── scenes/         # Scene/state headers (future)
│
├── src/                    # Source files
│   ├── engine/             # ECS engine implementations
│   │   ├── EntityManager.cpp # Entity management implementation
│   │   ├── ComponentStorage.cpp # Component storage implementation
│   │   ├── ComponentManager.cpp # Component management implementation
│   │   ├── System.cpp      # System base class implementation
│   │   ├── SystemManager.cpp # System management implementation
│   │   └── Registry.cpp    # ECS registry implementation
│   │
│   ├── game/               # Game-specific implementations
│   │   ├── components.cpp  # Component implementations (if needed)
│   │   ├── systems/        # System implementations (future)
│   │   ├── entities/       # Entity setup code (future)
│   │   └── scenes/         # Scene logic (future)
│   │
│   ├── Entities/           # Legacy code (to be migrated)
│   │   ├── GameObject.h
│   │   ├── Player.h
│   │   ├── Laser.h
│   │   └── Meteor.h
│   │
│   ├── Game.h              # Main game class
│   ├── Game.cpp
│   └── main.cpp            # Entry point
│
├── build/                  # Build artifacts
├── docs/                   # Documentation
├── images/                 # Game assets
└── CMakeLists.txt          # Build configuration
```
