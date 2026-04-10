# Game Design Document — *MB Remote Maintenance Terminal*

---

## 1. Overview

| | |
|---|---|
| **Title** | MB Remote Maintenance Terminal *(working title)* |
| **Genre** | Puzzle / Narrative |
| **Platform** | PC — Windows, Linux, macOS (WASM/browser stretch goal) |
| **Scope** | Solo jam project, possible future release |
| **Engine** | C + raylib |

**Logline:** The player finds an old '80s computer and boots it up, only to discover it is a remote maintenance terminal connected to a secret lunar weapons platform — the life's work of a paranoid billionaire, now apparently unmanned, online, and locked on target.

---

## 2. Premise & Setting

A deranged billionaire, convinced an alien invasion was imminent, secretly commissioned and built a defense station on the Moon, armed with a massive weapon controlled by a non-anthropomorphic AI. The project was run on strict "need to know" access — each team only knew their own module.

The player stumbles onto an old terminal. They don't know what they've found. The manual is so heavily redacted it's almost useless. A message on the system from a scientist — complaining to a manager that the secrecy makes the documentation worthless — is among the first things they'll read.

**Tone:** Dark humor with a thriller undercurrent. Dry, bureaucratic horror. The impending doom is real, but presented through the lens of corporate satire and institutional absurdity.

---

## 3. Core Gameplay Loop

1. **Connect** to a module via `connect <module>`
2. **Diagnose** components using generic and module-specific commands
3. **Repair** components to restore module integrity
4. **Explore** available documents (logs, emails, manuals) — some accessible immediately, some requiring repair or decryption
5. **Route power** to the next relevant module and repeat
6. **Piece together** the story and decide the fate of the system

The player is always navigating a hierarchy: **Terminal → Module → Component**, with commands contextually available depending on the current level.

---

## 4. Modules & Components

The system is composed of **modules**, each made up of **components** (e.g. `POWER`, `NETWORK`, `FS`). The terminal itself is a module.

**Component types (current/planned):**

| Component | Role |
|---|---|
| `POWER` | Required for the module to function |
| `NETWORK` | Enables inter-module communication |
| `FS` | Filesystem — gates access to documents |

More modules and component types to be defined. All modules are initially unknown to the player.

---

## 5. Command System

Commands follow a context-sensitive model:

- **Generic** (available everywhere): `help`, `status`, `connect <module>`, `disconnect`
- **Module-level**: module diagnostics, repair commands, power routing
- **FS component**: `list`, `open <file>`, `repair <file>`, `decrypt <file>`

Some commands must be discovered through gameplay (e.g. found in logs, deduced from partial documentation). Figuring out undocumented commands is part of the puzzle.

---

## 6. Puzzles

| Puzzle type | Description |
|---|---|
| **Power routing** | The auxiliary power unit can only power one module at a time. The player must route power in the correct sequence to make progress. Until the main generator is repaired, this is a constant constraint. |
| **Repair sequencing** | Some modules depend on others being online first. Finding the right activation order is the central meta-puzzle. |
| **Command discovery** | Some interfaces have undocumented commands the player must infer from logs or manuals. |
| **Parameters & values** | Certain repairs require providing correct values (frequencies, codes, access levels). |
| **Cryptography** | Some documents are encrypted with simple ciphers and must be decoded. |

Documents provide story context and puzzle hints, but are never mandatory for progression — a player can brute-force or ignore them, at the cost of missing the narrative and making puzzles harder.

---

## 7. Documents & Story Delivery

All documents are deliberately fragmented. Information is censored per need-to-know access — each team's files reference their work only, with everything else redacted. The full picture emerges only by cross-referencing documents across multiple modules.

**Document access tiers:**
- Available on connection (no repair needed)
- Unlocked after component repair
- Locked behind decryption

The story is told entirely through emails, logs, technical manuals, and memos — never through cutscenes or dialogue.

---

## 8. Endings

| Ending | Condition | Description |
|---|---|---|
| **Bad** | Fully repair the system, miss or ignore the targeting problem | The AI executes its original program. Earth is attacked. |
| **Neutral** | Repair the system + find and complete the targeting reprogramming puzzle | The weapon is redirected away from Earth. The station remains operational. |
| **Good** | Repair enough of the system + find the self-destruct sequence | The station is destroyed. The threat is eliminated permanently. |

---

## 9. Aesthetic & UX

- **Visual style:** Authentic '80s terminal aesthetic — monospace font, CRT scanline shader, color palettes (green/amber phosphor), no mouse required.
- **Audio:** TBD
- **Accessibility:** On-screen keyboard as stretch goal (required for browser build).

---

## 10. Out of Scope (for jam)

- WASM/browser build
- On-screen keyboard
- Audio
- Animations beyond the existing CRT scanline scroll
