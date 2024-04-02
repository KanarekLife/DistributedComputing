# Tanks - dokumentacja

Generowanie diagramu sekwencji:

```bash
mmdc --theme default --backgroundColor transparent --input diagrams/sequence.mmd --output imgs/sequence.png
```

Generowanie diagramu klas:

```bash
mmdc --theme default --backgroundColor transparent --input diagrams/class.mmd --output imgs/class.png
```

Tryb deweloperski:

```bash
typst watch --font-path fonts main.typ
```

Kompilacja do PDF:

```bash
typst compile --font-path fonts main.typ
```
