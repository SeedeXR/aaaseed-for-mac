# Hydraaa MEU : guide du panneau de configuration

Le panneau du MEU Hydraaa est divisé en deux colonnes :
- gauche (x = 1..8) : contrôles spécifiques au transpileur Hydra
- droite (x = 9..16) : contrôles MEU standards (camera, transfo, blending...)

Plus une bande haute (`add_shading_ui`) qui couvre les 8 cellules pour gérer le shader généré.

---

## Bandeau du haut

`add_shading_ui` , la barre de gestion du shader. Permet de :
- choisir le fichier shader actif (vert, frag, geom) à éditer
- recharger, sauver, réinitialiser le shader
- compteur d'erreurs de compilation

Le file selector ici n'est pas le sélecteur Hydra (voir plus bas), c'est celui d'AAASeed pour ouvrir le shader généré dans un éditeur externe par exemple.

---

## Colonne gauche : contrôles Hydraaa

### Row 1, time
- `Time Active` (bouton on/off, 1 cell) : démarre ou met en pause l'animation. Activé par défaut.
- `Time` (slider, 5 cells) : valeur de temps en secondes, range [0, 1000]. Si Time Active est ON, s'incrémente de `aaa.time.dt` chaque frame. Cette valeur est poussée dans `aaa_fu_float[0]`, exposée comme `time` dans le code Hydra.
- `Restart` (trig, 2 cells) : remet `time` à 0.

### Row 2, sélecteur de fichier .txt
- `Hydra File` (dropdown, 7 cells) : liste tous les fichiers `.txt` du dossier du MEU. Clic ouvre la liste, sélection re-transpile le fichier et écrit le résultat dans `sha.frag` (que AAASeed recompile au prochain frame via mtime watch).
- `Refresh` (trig, 1 cell) : rescanne le dossier du MEU. Utile si tu drop un nouveau fichier `.txt` pendant que AAASeed tourne.

### Row 3, FFT bins audio
- `fft0`, `fft1`, `fft2`, `fft3` : 4 sliders range [0, 1], 2 cells chacun. Servent pour le code Hydra qui utilise `() => a.fft[0..3]` (style audioreactive). Mappés sur `aaa_fu_float[5..8]`. Pour de la vraie réactivité audio, bind chaque slider sur un contrôleur MIDI ou un canal OSC. Tu peux aussi les bouger à la main pour tester.

### Row 4, FBO target (feedback)
- `FBO target` (champ texte, 6 cells) : nom du MEU FBO dans ta scène vers lequel rediriger le rendu Hydraaa. Si vide, le rendu va direct dans le FBO du show comme un MEU classique. Si rempli, le rendu va dans la texture du MEU FBO nommé.
- `Apply FBO` (trig, 2 cells) : applique la valeur saisie. Appelle `set_meu_fbo(name)` puis chaque frame `do_fbo()` est exécuté avant `draw_layer`. Active aussi le binding de la texture du FBO sur `g_fbo_out` (binding 4) pour que le code Hydra `src(o0..o3)` lise la frame précédente, vraie boucle feedback.

### Rows 5, 6, 7, 8, textures d'entrée
- 2 lignes de 2 cellules : `Tex_1`, `Tex_2`, `Tex_3`, `Tex_4`. Chacun ouvre un sélecteur de bank/bind 2D au clic. Les 4 slots sont bindés sur GL_TEXTURE 0, 1, 2, 3, accessibles dans le code Hydra via `src(s0)`, `src(s1)`, `src(s2)`, `src(s3)` qui correspondent aux samplers `g_input_texture_0` à `g_input_texture_3`.

---

## Colonne droite : contrôles MEU standards

### Row 1, caméra
- `add_camera` : bouton pour switcher entre les caméras numérotées du MEU. Tu peux modifier la pose de chacune dans la barre du haut quand elle est sélectionnée. Pour Hydraaa, le rendu est full-screen NDC, la caméra n'a quasiment aucun impact visuel.

### Row 2, taille (3 cells de haut)
- `add_size_uvf_video` : trois sliders qui définissent la taille en pixels du buffer de sortie quand le MEU rend dans son propre FBO (largeur, hauteur, format de channels).

### Row 3, transfo (3 cells)
- `add_transfo` : sliders **TRS** (translate, rotate, scale). Transforme le quad final dans la scène. Affecte la position et la taille du dessin Hydra quand il est composité dans le show via le système des render rectangles.

### Row 4, mapping
- `add_mapping_by_side_only` : sélecteur du mode de mapping de texture (par face, projection, répétition...). Pour Hydraaa qui rend un quad full-screen, l'effet est marginal.

### Row 5, RGBF
- `add_rgbf` : 4 sliders R, G, B, F (multiplicateurs RGBA appliqués à la couleur finale du layer). Dans la version actuelle, le shader fait `out_result = c00` directement, donc ces sliders n'ont pas d'effet visible. Latent.

### Row 6, blending
- `add_blending` : mode de blending pour la composition du MEU dans le show (alpha blend, additif, multiply, etc.).

### Row 7+, rendering
- `add_rendering` : réglages de rendu du layer : depth test, cull mode, line size, point size.

---

## Onglets supplémentaires en bas

- `add_shading_sliders_tab Vert / Frag` : onglets pour exposer les uniformes brutes du shader (`aaa_fu_float[N]`, ints, vec4...). Chaque slider tweake une uniform à chaud sans recompile. Note : les uniforms 0..8 sont déjà occupés par time, mouse, resolution, fft0..3. Si tu modifies les sliders qui pointent dessus, tu écrases la valeur poussée chaque frame par `update()`.

---

## Champs internes (pas dans l'UI)

| nom | type | rôle |
|---|---|---|
| `self.s_hydra_file` | int | index du fichier .txt sélectionné dans le menu |
| `self.b_time` | bool | flag time active |
| `self.time` | float | temps courant en secondes |
| `self.fft_0..3` | float [0,1] | valeurs des FFT bins audio |
| `self.__hydra_files` | table | cache de la liste des .txt présents |
| `self.__fbo_target` | string ou nil | nom du MEU FBO cible (si feedback activé) |
| `self.__last_sel` | int | dernière valeur de `s_hydra_file` polled, sert à détecter le changement |
| `self.__last_src` | string | dernier contenu .txt lu, sert au hot reload du fichier |

---

## Ajouter un nouveau contrôle

C'est dans `meu:define_ui()` qu'on pose les widgets. Le pattern :
- `self:add_slider({ix,iy, sx,sy}, "label", self, "field_name", default, min, max)` : un slider qui écrit dans `self.field_name`.
- `self:add_button({ix,iy, sx,sy}, "label", self, "field_name", default)` : un bool toggle.
- `self:add_text({ix,iy, sx,sy}, "label")` : champ texte éditable.
- `self:add_trig_method({ix,iy, sx,sy}, "label", self, "method_name")` : bouton qui appelle une méthode.

Pour exposer un nouveau float au shader, ajoute aussi un `sha:set_frag_float(N, value)` dans `meu:update()` et un `aaa_fu_float[N-1]` correspondant dans le préambule du transpiler ou dans le code Hydra.
