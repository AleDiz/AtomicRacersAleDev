<p align="center">
  <img src="https://github.com/AleDiz/AtomicRacersAleDev/blob/main/img/logo_atomic.png" alt="Atomic Racers Logo" width="400">
</p>

**Project Role:** Programmer & Team Lead  
**Tech Stack:** Custom C++ Engine | ECS Architecture | Utility AI  
**Team Size:** 5 Members (9-month development cycle)

*Atomic Racers is a high-speed arcade racing experience developed on a custom-built C++ engine. The project emphasizes fluid vehicle physics, dynamic power-up systems, and an unpredictable AI designed to challenge the player's strategic use of abilities.*

---

## 🕹️ Play the Game
**The latest stable build is available on itch.io:** 👉 [**Play Atomic Racers on itch.io**](https://empiric-team.itch.io/atomic-racers)

---

## 🛠️ Technical & Design Overview
The primary challenge of this project was building a cohesive racing experience from scratch without a commercial engine. As the Lead, I directed the holistic design vision while engineering the core technical architecture. I ensured the whole game remained decoupled from the graphics engine, facilitating a smooth transition from RayLib to our custom graphics API. I also stepped into the role of AI designer and developer, balancing challenge and fun to maximize player engagement.

---

## 👨‍💻 Key Systems & My Contributions
Our custom C++ engine follows an ECS (Entity Component System) architecture. In this data-driven structure, entities are collections of data, components store that data, and systems process it. While I contributed to several areas of the engine, I specialized in the following two systems:

### 🧠 Utility-Based AI System
Instead of using traditional Finite State Machines (FSM), I engineered a Utility AI to create organic and competitive opponent behavior.
* **Logic:** AI agents evaluate multiple environmental variables (obstacles, distance to other cars, available power-ups, distance to the next waypoint) through a scoring matrix.
* **Implementation:** Used mathematical response curves to normalize inputs, allowing for more "human-like" decision-making compared to rigid IF-THEN logic.
* **🔗 Code Highlight:**
  * [UtilityAI.cpp](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/src/util/UtilityAI.cpp) — *Scoring loop and action selection logic.*
  * [AISystem.cpp](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/src/system/AISystem.cpp)  — *Execution of AI actions.*
  * [AIComponent.hpp](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/src/components/AIComponent.hpp)  — *AI data structure.*

### 🎨 Custom Rendering & ECS Integration
I developed the underlying architecture to handle rendering within our custom ECS.
* **Implementation:** Designed a decoupled render system that could potentially draw the game in any graphics engine while remaining performant.
* **Architecture:** Built upon the Facade Pattern. The RenderManager relies on a GraphicsAPI virtual class. Entities possess a RenderComponent which is processed by the RenderSystem and passed to the RenderManager for drawing.
*  **[Render Structure Diagram](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/img/RenderComponentDiagram.png)**
* **🔗 Code Highlight:** 
  * [RenderManager.cpp](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/src/man/RenderManager.cpp) — *Usage of GraphicsAPI to render the game.*
  * [GraphicsAPI.hpp](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/src/engine/GraphicsAPI.hpp) / [RaylibAPI.cpp](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/src/engine/RaylibAPI.cpp)  — *The facade for graphics engines and the Raylib implementation.*
  * [RenderSystem.cpp](https://github.com/AleDiz/AtomicRacersAleDev/blob/main/src/system/RenderSystem.cpp) — *The bridge between the ECS and the RenderManager.*

---

## 📈 Leadership & Methodology
As the **Team Lead**, I was responsible for the project's health and delivery:
* **Agile Workflow:** Implemented Scrum methodologies, managing 2-week sprints and daily stand-ups to maintain a 9-month development roadmap.
* **Technical Vision:** Acted as the bridge between art and code, ensuring that artistic assets were integrated efficiently into our custom engine's technical constraints.

---

### 📩 Contact & Feedback
* **GitHub:** [AleDiz](https://github.com/AleDiz)
* **LinkedIn:** [Alejandro Díaz](https://www.linkedin.com/in/alejandro-diaz-alcaraz-037099242)
* **Email:** aledevgames@gmail.com

---

Note: As this was a collaborative project developed within a Spanish-speaking academic environment, some internal code comments remain in Spanish, though the core architecture and documentation are presented in English.
