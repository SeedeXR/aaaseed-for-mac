# Hydraaa MEU : guide pour Claude Code

MEU AAASeed qui interprète du code [Hydra](https://hydra.ojack.xyz) et génère un GLSL fragment shader équivalent. Inspiré de [HeadlessHydra](https://codeberg.org/gugray/HeadlessHydra) pour les bodies GLSL des opérateurs.

But : permettre du copy-paste de code Hydra trouvé sur internet ou écrit dans hydra.ojack.xyz, et le rendre dans AAASeed sans modification.

## Vue d'ensemble

```
.txt Hydra source ─► hydraaa_transpile.lua (parser + codegen)
                  ─► sha.frag (GLSL généré, ré-écrit à chaque sélection)
                  ─► AAASeed file watcher détecte le mtime change
                  ─► sha.frag est recompilé sur la frame suivante
                  ─► applique au quad fullscreen rendu par hydraaa.lua
```

Le MEU est un shader-rendering MEU classique avec une seule layer (Layer_A) qui sert juste à set up les états (camera, color, shading binding) ; le draw visible est un `aaa.draw_rect_uv(-1,-1,1,1)` manuel après le `draw_layer(1)`. Pattern repris de TutoLuaDrawPrimitive et TutoMapOSM.

## Fichiers

| fichier | rôle |
|---|---|
| `fx.aaa_layers_all` | arbre objet AAASeed (layer + texturing + shading + camera) |
| `hydraaa.lua` | MEU script (UI, init, update, draw) |
| `hydraaa_transpile.lua` | transpiler Hydra to GLSL en pur Lua |
| `sha.vert` | vertex shader minimaliste, interface block VS_out |
| `sha.frag` | fragment shader généré (placeholder à l'init, écrasé par le transpiler) |
| `*.txt` | exemples de code Hydra sélectionnables dans l'UI |
| `UI.md` | documentation utilisateur du panneau de configuration |
| `CLAUDE.md` | ce fichier |

## Ce qui marche (V2)

### Transpiler

- Tous les opérateurs Hydra de base (52 ops) : sources (osc, gradient, noise, voronoi, shape, solid, src, prev), color (brightness, contrast, hue, colorama, posterize, shift, r/g/b/a, invert, luma, thresh, color, saturate), coord (rotate, scale, pixelate, repeat, repeatX, repeatY, kaleid, scroll, scrollX, scrollY), combine (add, sub, mult, blend, diff, layer, mask), modulator (modulate, modulateScale, modulateRotate, modulateRepeat, modulateRepeatX, modulateRepeatY, modulateKaleid, modulateScrollX, modulateScrollY, modulatePixelate, modulateHue).

- Sémantique de chaîne : coord ops appliqués sur `st` en ordre INVERSE (le dernier `.rotate()` chaîné s'applique en premier), source sample, color ops appliqués en ordre direct sur `c`. Sub-chaînes des modulateurs et blends rendues dans des namespaces `st0N/c0N` uniques (compteur global).

- Arrow functions `() => expr` parsées comme l'expression elle-même (Hydra utilise ça pour passer des valeurs dynamiques).

- `a.fft[0..3]` reconnu, mappé sur 4 sliders UI dans le panneau du MEU. Permet d'écrire `osc(40, () => a.fft[0] * 4)` style audio-réactif.

- Expressions arithmétiques `+ - * /` avec précédence standard. `20 + a.fft[0] * 60` parse correctement comme `20 + (fft * 60)`.

- Arrays `[v1, v2, v3].smooth(s).fast(f)` compilées en GLSL avec interpolation temporelle. Cycle à fréquence `fast`, smooth contrôle l'amount d'interpolation entre indices consécutifs.

- Définitions de fonction `name = () => chain`, stockées dans une table de symboles, expandées inline à chaque usage de `name` ou `name()`. Récursion limitée à 8 pour prévenir les boucles.

- Top-level skip : `speed = 1`, `a.setSmooth(0.96)`, et autres assignments / méthodes globales Hydra ignorés silencieusement.

- Robustesse : statement non parseable saute jusqu'au prochain stmt valide (un mauvais bout ne casse pas le programme entier).

- Sub-chaînes parallèles (deux modulates ou deux blends au même niveau) ont chacune un index unique grâce au compteur global passé via `idx_counter[1]`.

### MEU shell

- 4 textures user-bindées via la grille `Tex_1..4` du MU. Chacune est mappée sur `g_input_texture_0..3` (binding 0..3) via `bind_texture_to_unit` appelé en draw, code Hydra accède via `src(s0..s3)`.

- 4 sliders FFT bins `fft0..fft3` poussés chaque frame dans `aaa_fu_float[5..8]`, code Hydra y accède via `() => a.fft[N]`. Bind-les MIDI/OSC pour de l'audio-réactivité réelle.

- Time slider + Restart pousse `aaa_fu_float[0]` (`time` en GLSL) et l'incrémente avec `aaa.time.dt` quand `b_time` est ON.

- Mouse et resolution poussés dans `aaa_fu_float[1..4]` chaque frame (best effort, defaults à 0 et 1 si l'API ne répond pas).

- Hot reload du fichier `.txt` sélectionné : update() poll le contenu toutes les ~60 frames, re-transpile si modifié. Édite le `.txt` dans n'importe quel éditeur, sauve, et le shader change live en ~1s.

- Selecteur de fichier en dropdown (pattern PROTO_Out : add_button + set_menu + set_target_lua, ordre exact requis sinon le menu n'écrit pas back). Liste tous les `.txt` du dossier MEU. Polling de `s_hydra_file` dans update() pour détecter les changements (set_method_on_value_change ne fire pas fiablement avec ce pattern).

- FBO target output : champ texte + bouton Apply. Si rempli avec le nom d'un MEU FBO de la scène, active `set_meu_fbo(name)` à init et `do_fbo()` chaque frame. La texture du FBO est bindée sur GL_TEXTURE4 (sampler `g_fbo_out`) chaque frame, donnant accès à la frame précédente via `src(o0..o3)` dans le code Hydra. Vraie boucle feedback Hydra-style.

## Patterns Mac-spécifiques importants

### sha.vert : interface block obligatoire

Sur Apple GL Mac, `layout(location = N) out vec2 var;` dans un vertex shader déclenche une validation transform-feedback qui échoue avec `MAX_TRANSFORM_FEEDBACK_BUFFERS = 0`. Le seul shader MEU qui compile sur Mac, `Shader/GaBu_Monitor.vert`, utilise un interface block sans `layout(location)` ni qualifier `flat`. C'est ce pattern qu'on a copié pour `sha.vert` :

```glsl
out VS_out {
    vec4 color;
    vec2 tex_coor;
} vs_out;
```

Côté frag, le pendant `in VS_out { ... } vs_in;` matche par nom de bloc.

ExShaderGrid, ExShaderInstance, CartoMaton et autres MEUs shader-based ne compilent pas sur Mac actuellement à cause de ce problème xfb_buffer. Voir tâche `#20 Investiguer xfb_buffer pour MEU shaders Mac` dans le backlog.

### draw_layer ne dessine pas la géométrie

Sur Mac, `draw_layer(N)` ne fait QUE configurer l'état GL (camera, color, shader binding). La géométrie de la layer (`bdd_grid`) est invisible (rendue mais hors cadrage ou avec UVs zéro). Pour que le shader produise un visible, il faut faire un draw manuel APRÈS `draw_layer(N)`. Pattern utilisé : `aaa.draw_rect_uv(-1,-1,1,1)` qui dessine un quad fullscreen NDC avec UVs 0..1 (gl_MultiTexCoord0).

`aaa.draw_rect` (sans `_uv`) ne pousse pas les UVs, le shader voit `gl_MultiTexCoord0 = (0,0)` et tout sample uv-dépendant retourne du noir. **Toujours `draw_rect_uv` ici.**

### Texture binding manuel

Bindings auto par la layer ne persistent pas jusqu'à notre `draw_rect_uv`. On rebinde manuellement chaque frame avec `gol.set_tex_unit_2d_bind(unit, bind)` (via `bind_texture_to_unit(id, unit-1)` pour les Tex_1..4 et un binding direct pour le FBO sur unit 4).

### bdd_boxes stack canary

Pas lié à Hydraaa mais à signaler : `c_bdd_boxes::get_mouse_uv` à `bdd_boxes.cpp:751` plantait avec `__stack_chk_fail` parce que `REAL uv[2]` était trop petit pour `coor_camera_to_world` qui écrit 3 floats. Fix appliqué dans une session précédente, `uv[3]` avec `uv[2] = 0`.

### Driver Mesa+KK : padding vec2/vec3 to vec4

Voir `mac_port/mesa_kk_backup/README.md` et la mémoire `mesa-vbuf-vec4-padding.md`. Le scramble des sliders MIDI sur Apple Silicon Metal vient d'une race CPU/GPU sur stride alternant. Fix dans `mesa-main/src/gallium/auxiliary/util/u_vbuf.c`, force vec1/vec2/vec3 to vec4 au staging time, `__APPLE__` only.

## Limitations connues / TODO

- **xfb_buffer générique pour les autres MEU shaders Mac** (tâche #20). Hydraaa s'en sort avec son interface block. Pour ExShaderGrid et autres, il faudrait soit un fix Mesa côté driver, soit une migration des templates de tous les vert shaders MEU vers le pattern interface block.

- **`.smooth(N)` sur autre chose qu'un array** ignoré. Hydra permet `chain.smooth(N)` pour appliquer un smoothing sur la sortie de la chaîne (filtre temporel). Pas implémenté en V2.

- **Méthodes Hydra exotiques** ignorées : `.ease(...)`, `.offset(...)`, `.fit(...)`, `.repeat(...)` sur les arrays.

- **Pas de vraie pingpong FBO** : `src(o0)` lit le FBO target (donc la frame précédente du MEU lui-même), mais c'est du single-buffer pas du double-buffer. Sur Apple Metal, lire et écrire le même FBO dans la même frame est undefined ; en pratique on lit la frame précédente parce qu'AAASeed swap les FBOs entre frames. À tester en conditions réelles, peut faire des artefacts.

- **`src(o1)`, `src(o2)`, `src(o3)`** mappés tous sur le même `g_fbo_out`. Hydra prévoit 4 buffers de sortie séparés ; on simule avec un seul.

- **Pas de FFT réel auto-piloté** : les 4 sliders fft sont user-controllés. Pour brancher du vrai FFT, il faudrait C++ qui calcule les bins depuis `c_snd_master` et les pousse dans les sliders chaque frame. Pattern existant dans `Shader/WebGL/Filters/AudioWave.txt` qui sample `aaa_tex2d[0]` pour lire un FFT bind en texture, à étudier pour réutiliser.

## Backup

V1 (avant les ajouts arrays + fn defs + FBO target) sauvegardée dans :
`/Users/xpo/DEV/AAASF/macseed/Hydraaa_v1.zip`

Restore avec `unzip -o Hydraaa_v1.zip -d <dest>/AAA_PROTO/MEU_PROTO_2/`.

## État au 2026-04-25

- Toutes les features V2 implémentées et testées : skip top-level, function defs, arrays smooth/fast, FBO target output.
- Le code eerie_ear `10_eerie_ear_vhs.txt` transpile à 13K de GLSL valide.
- Multi-textures s0..s3 fonctionnelles, FBO feedback en place.
- Hot reload des `.txt` actif.
- Restart d'AAASeed nécessaire après modif de `fx.aaa_layers_all`. Modifs du `.lua` et `.txt` reloadent à chaud.
- Le MU tile s'affiche dans `render_2` de `AAA_MUS.bus` à la position {2.5,-0.5} avec taille agrandie 3x4.5 (modifié en session pour mieux voir).

## Comment ajouter un opérateur Hydra non supporté

Si tu rencontres un op qui manque, par exemple `bouncer` ou `lumapix` :

1. Ajouter une entrée dans la table `OPS` à la racine de `hydraaa_transpile.lua` :
```lua
bouncer = { kind=OP_KIND_COLOR, args={ arg("amount",0.5) } }
```

2. Ajouter le body GLSL dans `GLSL_OP_BODIES` (copier-coller depuis hydra-synth ou HeadlessHydra) :
```lua
bouncer = [[
vec4 bouncer(vec4 _c0, float amount) {
    return vec4(...);
}]],
```

3. Tester avec un `.txt` simple qui utilise l'op, vérifier que le shader compile.

## Patterns à suivre dans hydraaa.lua

Si tu modifies le MEU script, respecter :
- `aaa.lua.global.declare_table("HYDRAAA_TRANSPILE")` AVANT toute lecture de `_G.HYDRAAA_TRANSPILE` (strict.lua AAASeed).
- Toujours `_G.HYDRAAA_TRANSPILE` (pas `HYDRAAA_TRANSPILE` direct) à la lecture, sinon strict.lua throw.
- Initialiser `self.s_hydra_file = self.s_hydra_file or 1` APRÈS `set_target_lua` (pattern PROTO_Out : sinon le menu ne write pas back).
- Sync proto et instance manuellement après modif de `fx.aaa_layers_all` ou `hydraaa_transpile.lua` sinon l'instance reste sur l'ancien : `cp proto/file instance_dir/`.
- Pour les changements de `fx.aaa_layers_all`, restart AAASeed (pas de hot reload de cette structure).
