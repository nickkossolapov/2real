# SOCD

**Simultaneous Opposing Cardinal Directions** is the problem of deciding what an axis should do when both opposing keys are held at once (Left *and* Right, Up *and* Down). The keyboard can report both as down; the analog stick can't, so this only comes up for digital input feeding a `move` axis.

There's no single correct answer, just policies:

- **Neutral** - both down cancels to `0`. Common in fighting games (prevents impossible left+right).
- **Last input wins** - the most recently pressed direction takes over. Feels best for WASD movement (Quake/Source style).
- **First wins** - the direction held first keeps priority. Rare, feels sticky.

