# Atomic Racers 🏎️

**Project Role:** Technical Game Designer & Team Lead  
**Tech Stack:** Custom C++ Engine | ECS Architecture | Utility AI  
**Team Size:** 5 Members (9-month development cycle)

*Atomic Racers is a high-speed arcade racing experience developed on a custom-built C++ engine. The project emphasizes fluid vehicle physics, dynamic power-up systems, and an unpredictable AI designed to challenge the player's strategic use of abilities.*

---

## 🕹️ Play the Game
**The latest stable build is available on itch.io:** 👉 [**Play Atomic Racers on itch.io**](https://empiric-team.itch.io/atomic-racers)

---

## 🛠️ Technical & Design Overview
The primary challenge of this project was building a cohesive racing experience from scratch without a commercial engine. As the **Lead**, I directed the holistic design vision while engineering the core technical architecture to ensure that gameplay systems (like the Utility AI and UI flow) remained decoupled, performant, and easy to iterate upon.

---

## 👨‍💻 Key Systems & My Contributions

### 🧠 Utility-Based AI System
Instead of using traditional Finite State Machines (FSM), I engineered a **Utility AI** to create organic and competitive opponent behavior.
* **Logic:** AI agents evaluate multiple environmental sensors (track curvature, player distance, available power-ups) through a scoring matrix.
* **Implementation:** Used mathematical response curves to normalize inputs, allowing for more "human-like" decision-making compared to rigid IF-THEN logic.
* **🔗 Code Highlight:** [`UtilityAI_System.cpp`](TU_URL_AQUI) — *Check the scoring loop and action selection logic.*

### 🎨 Custom Rendering & ECS Integration
I developed the underlying technical architecture to support the core gameplay loop within our custom ECS.
* **Implementation:** Designed the data-oriented rendering pipeline to handle high-speed entity updates without performance bottlenecks.
* **Architecture:** Focused on memory locality within the ECS to ensure that the Render System and Physics System could access component data with minimal cache misses.
* **🔗 Code Highlight:** [`Renderer.cpp`](TU_URL_AQUI) — *View the core frame rendering logic.*

### 🖥️ Technical UI/UX Flow
I took ownership of the complete UX/UI flow, building a robust architecture to handle state transitions.
* **Implementation:** Developed an Event-Driven system to decouple the UI from the game's core logic, ensuring that menu transitions and HUD updates did not interfere with racing performance.
* **🔗 Code Highlight:** [`UIManager.cpp`](TU_URL_AQUI) — *See the event-listener pattern implementation.*

---

## 📈 Leadership & Methodology
As the **Team Lead**, I was responsible for the project's health and delivery:
* **Agile Workflow:** Implemented Scrum methodologies, managing 2-week sprints and daily stand-ups to maintain a 9-month development roadmap.
* **Technical Vision:** Acted as the bridge between art and code, ensuring that artistic assets were integrated efficiently into our custom engine's technical constraints.

---

## 📸 Technical Showcase
*(Tip: Add a GIF here showing the AI pathfinding or a screenshot of your debug console showing FPS/ECS metrics)*
![Gameplay Screenshot](TU_URL_A_IMAGEN_O_GIF)

---

### 📩 Contact & Feedback
* **GitHub:** [AleDiz](https://github.com/AleDiz)
* **LinkedIn:** [Alejandro Díaz](https://www.linkedin.com/in/alejandro-diaz-alcaraz-037099242)
* **Email:** aledevgames@gmail.com
