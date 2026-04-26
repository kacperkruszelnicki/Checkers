# Checkers Game (C++ / SDL3)

A fully functional checkers (draughts) game implemented in C++ using SDL3. The project features a graphical user interface, basic animations, and an AI opponent.

## 🎮 Features

* Classic 8x8 checkers gameplay
* Player vs AI mode
* Move validation (including captures and multi-captures)
* King promotion mechanics
* Animated piece movement
* Visual hints for possible moves
* Game over detection (win/lose conditions)
* Restart option after game ends

## 🧠 AI

The game includes a simple AI opponent that:

* Evaluates possible moves
* Selects the best move based on board state
* Supports capturing logic and prioritization

## 🖥️ Technologies Used

* **C++**
* **SDL3**
* **SDL3_image**
## ⚙️ Requirements

* C++ compiler (C++17 or newer recommended)
* SDL3
* SDL3_image

## 🎯 Controls

* **Mouse Left Click** – Select and move pieces
* **R key** – Restart game (after game over)

## 🧩 Game Rules

* Standard checkers rules apply
* Captures are mandatory
* Multiple captures are supported
* Pieces are promoted to kings upon reaching the opposite side

## 📸 Assets

Make sure the following files exist in the `assets/` folder:

* `white_piece.png`
* `black_piece.png`
* `white_king.png`
* `black_king.png`
* `gray_piece.png`
* `win_text.png`
* `lose_text.png`
