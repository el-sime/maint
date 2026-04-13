# Plan: Text Reading Functionality (maint)

## TL;DR
Implement a `read <filename>` command that loads a text file from `assets/text/` and displays it in a paginated "reading mode" that temporarily takes over input handling. Controls are configurable via constants. A page counter is shown in the status bar.

## Decisions
- Pagination: show one "page" at a time (N lines = maxOutputLines), SPACE/ENTER advance, arrow keys scroll line, Q exits
- Command: `read <filename>`
- Files: `assets/text/` copied to build via existing CMake post-build step
- Redaction: baked into text files (static)
- Controls: configurable via constants.h
- File listing: out of scope for this plan

---

## Steps

### Phase 1 — Constants
1. Add key binding constants to `constants.h`:
   - `READ_KEY_NEXT_PAGE`, `READ_KEY_PREV_PAGE`, `READ_KEY_SCROLL_UP`, `READ_KEY_SCROLL_DOWN`, `READ_KEY_EXIT`

### Phase 2 — Reader State (globals in main.c)
2. Add reader state globals:
   - `static bool isReading` — whether reading mode is active
   - `static char *readBuffer` — heap-allocated contents of opened file
   - `static int readTotalLines` — total line count in document
   - `static int readCurrentLine` — top line of current view
   - `static int readLinesPerPage` — = maxOutputLines (computed at Init)

### Phase 3 — File Loading
3. Add `LoadTextFile(const char *path)` → returns `char *` (heap, caller frees). Returns NULL on failure. Path is relative to exe: `assets/text/<filename>`.

### Phase 4 — ParseCommand
4. Add `read` case in `ParseCommand`:
   - Validate noun is present
   - Build path: `assets/text/<noun>`
   - Call `LoadTextFile`, set globals, set `isReading = true`
   - On error: AppendOutput error message

### Phase 5 — Update (reading mode input)
5. In `Update()`, add an early-return branch when `isReading == true`:
   - Check `READ_KEY_NEXT_PAGE` / `READ_KEY_PREV_PAGE` → advance/retreat by `readLinesPerPage`
   - Check `READ_KEY_SCROLL_DOWN` / `READ_KEY_SCROLL_UP` → advance/retreat by 1
   - Check `READ_KEY_EXIT` → set `isReading = false`, free `readBuffer`, clear readBuffer pointer
   - Clamp `readCurrentLine` to [0, readTotalLines - readLinesPerPage]
   - Update status bar text to "Page N/N"

### Phase 6 — Draw (reading mode)
6. In `Draw()`, when `isReading == true`, replace output text with the current page slice:
   - Extract lines `readCurrentLine` to `readCurrentLine + readLinesPerPage` from `readBuffer`
   - Render in place of normal output

### Phase 7 — Helpers
7. Add `ExtractLines(const char *buf, int startLine, int count, char *out, int outSize)` — copies N lines starting at startLine from a newline-delimited buffer into `out`.
8. `CountLines(const char *s)` — already exists, reuse.

---

## Relevant files
- `constants.h` — add key binding constants
- `main.c` — reader globals, LoadTextFile, ExtractLines, ParseCommand, Update, Draw

## Verification
1. `read lorem.txt` displays first page of lorem.txt
2. SPACE and arrow keys navigate correctly, page counter updates
3. Q exits reading mode, normal terminal resumes
4. `read nonexistent.txt` shows a clean error in output
5. Reading a file with fewer lines than a page shows it without crashing

## Further Considerations
1. **Asset packing** — for later: assets are currently plain files on disk; embedding them (e.g. via `xxd`-generated C arrays or a custom pack format) is a future hardening step, not needed now.
2. **File path flexibility** — should `read` support subdirectories (e.g. `read logs/entry1.txt`)? Easy to support with the same approach, but worth deciding before implementing path construction.
