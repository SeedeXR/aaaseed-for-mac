# Glossary

## GaBu
The first level of functionalities built in Lua on top of the C++ part of AAASeed.

## GaBuZoMeu
The second level of functionalities built in Lua on top of GaBu.  
This name comes from the French animation series *Les Shadoks* ([Wikipedia](https://en.wikipedia.org/wiki/Les_Shadoks), [Official Site](http://www.lesshadoks.com/)), used with authorization from [AAA Production](https://www.aaaproduction.fr/).

### GA
**Global Action**: Top-level utilities, including preferences, startup settings, help, window positioning, and UI elements. 

### BU
**Box User**: Generic class for all user-interactive objects (UI-enabled elements).

### BUS
Manages a collection of **BU** objects.

### MEU
**Module Editable Unit**: Encapsulates a functional block with UI and data.

### MU
**Module Unit**: Compact graphical version of a **MEU** for defining rendering order. Inherits from **BU**.

### MUS
Manages a collection of **MU** objects (similar to **BUS** for **BU**).

### APP
An **Application** in AAASeed (equivalent to a "document" or "patch").

### ZO
*Currently unused* ("*On a perdu les ZOs*"). *Planned for future updates.*

### UI
**User Interface**.

### Star Menu or UIF (for UI Fast)
Context menu triggered by left-click or quick-drag on any **BU** where click has other functions (e.g., `BUTTON`, `BU_MONITOR`).

---

# GABU_OBJ
**GABU_OBJ** is the base class from which most **GaBuZoMeu** objects are derived.  
**GABU_OBJ** supports methods for:
- Naming and labeling
- Class and documentation handling
- Printing and debugging
- Utilities, particularly for managing tree hierarchies

## Naming GABU_OBJ
Most functions (if not all) that create objects of class **GABU_OBJ** or derived classes require a name for the object. There are no constraints on the name - any string is valid. Names don't need to be unique and are primarily used for:
- Printing output
- Displaying dialogs
- Debugging purposes

```lua
-- Get the name of a GABU_OBJ 'o':
local name = o:get_name()

-- Change the name:
o:change_name(name)

-- Internal initialization (should only be used during creation):
self:__init_name(name)
```

---

# MEU / MU

- A **MEU** (Module Editable Unit):  
  Encapsulates a complete functional block including:  
  - Core functionality  
  - User interface components  
  - Associated data storage  

- A **MU** (Module Unit):  
  - Compact visual representation of a **MEU**  
  - Defines rendering order in the processing chain  
  - Grouped and managed by **MUS** container objects  

- **Inheritance**:  
  Both **MEU** and **MU** classes derive from **GABU_OBJ** base class

```lua
-- mu is a MU
-- meu is a MEU
local meu = mu:get_meu()
local mu = meu:get_mu()
```

## Naming MEU / MU

### Name Properties
- defined at creation.
- a MEU and its corresponding MU have the same name.
- Names can be modified via StarMenu's rename command
- Inherits naming basics from [GABU_OBJ](#naming-gabu_obj)

### **Valid characters:**
- Alphanumerics (`a-z`, `A-Z`, `0-9`)
- Symbols: Underscore (`_`), dot (`.`), dash (`-`)

```lua
-- Verify name compliance
MEU_CTX.cur:is_name_valid(name)  -- Returns boolean
```

### **Case Handling**:  
- Names are treated as lowercase internally:  
  `Displace_Example` ≡ `displace_example`  
- Uppercase preserved for display readability only

### **Name Structure**:  
- Format: `Type_Instance`  
  - `Type`: Text before first underscore (defines MEU class)  
  - `Instance`: Text after first underscore  

  **Examples**:  
  - `Displace_1` → Type: `Displace`, Instance: `1`  
  - `Ref_Fbo_F1` → Type: `Ref`, Instance: `Fbo_F1`  

### **Code**:  
```lua
-- Split name into components (helpers live on MEU_CTX):
local meu_type, inst = MEU_CTX.cur:split_meu_type_inst(name)

-- Reconstruct name from parts:
local full_name = MEU_CTX.cur:build_inst_name(meu_type, inst_key)

-- Validate a name against the allowed character set:
local b = MEU_CTX.cur:is_name_valid(name)

-- Instance methods (these are on the MEU itself):
local meu_type = m:get_meu_type()                 -- Get MEU type
local is_type  = m:is_meu_type(name)              -- Check type match
local instance = m:get_inst_key()                 -- Get instance key
local meu_type, inst = m:get_meu_type_inst_key()  -- Get both components
```

## MEU Name and Folder Uniqueness

### File System Structure
- Each **MEU/MU** maintains a corresponding physical folder
- Folder names exactly match their **MEU** names
- Root location: Application's `AAA_MEU/` directory
- Access: Use **Star Menu → Dir Button** to open in system explorer

### Directory MEUs (**MEU_DIR**)
- Special container-type MEUs that:
  - Organize other MEUs into folders
  - Maintain their own `AAA_MEU/` subfolder structure
  - Enable hierarchical organization

### Naming Rules
- **Same folder level**:  
  All names must be unique (file system constraint)
  
- **Different folders**:  
  Identical names are allowed  


## Finding MEU/MU by Name

### MEU-Level Search
```lua
-- m being a meu
-- name being a string or an array containing several possible names as string 
local meu = m:get_meu_by_name( name )
local meu = m:get_meu_by_name_no_error( name )
local mu  = m:get_mu_by_name( name )
local mu  = m:get_mu_by_name_no_error( name )
```  
**Search Behavior**:
- Checks current **MEU** level first  .
- Searches recursively through **MEU_DIR** folders (order undefined) .
- Moves up hierarchy if not found.
- Stops at APP level.

> **Note**: As of March 2025, path parsing requires manual implementation and can't be passed in the name.

### APP-Level Search
```lua
-- app is a lua global referencing the current application
-- name being a string or an array of strings
local meu = app:get_meu_by_name( name )
local meu = app:get_meu_by_name_no_error( name )
local mu  = app:get_mu_by_name( name )
local mu  = app:get_mu_by_name_no_error( name )
```  
These method also return the first MEU with the requested name. They act as the MEU method but start the search at the highest level of the hierarchy.

## Cached search methods
```lua
-- name being a string
-- app is a lua global referencing the current application
local meu, b_just_cached = app:get_meu_by_name_cached( name )
-- m being a meu
local meu, b_just_cached = m:get_meu_by_name_no_error( name )
```  

**Cache Behavior**:
- Stores found MEUs in cache for faster subsequent lookups  
- Automatically checks "freed" flag - re-searches if MEU was freed  
- Cache is maintained at either:  
  - Application level (for **APP** searches)  
  - **MEU** level (for **MEU** searches)  

**Return Values**:
- **meu**: The found MEU object (or nil if not found)  
- **b_just_cached**:  
  - `true`: MEU was just cached (first access)  
  - `false`: MEU was already cached  

> **Typical use**: Do Initialization when `b_just_cached` is true

---

# Labels

## Overview

- Labels are distinct from names because they are used purely for display purposes and do not affect object identification or searchability.
- Their primary function is to provide shortened UI text representations, making interfaces more readable and user-friendly.
- They are especially useful for MUs, where space constraints require compact display options.
- Unlike names, which must follow strict conventions for object reference, labels can be more descriptive and user-friendly.
- Persistence: Stored in each MEU's `aaa.lua` file.

## Label Code

```lua
-- Retrieve current label
local current_label = o:get_label()	-- Returns string

-- Set new label (any GABU_OBJ instance)
o:set_label("New Label")
```


# MEU Prototype / class
In AAASeed with the lua language we deal with object oriented programming using metamethods (you could say also metatables).
You, a user, don't need to know all the details of this: here is what you need to need to know.


## loading prototype
when an **APP** is loaded
- first the `AAA_PROTO` folder, in the Kernel folder (`AAAKernel`) is parsed. For any `MEU_PROTO_*` found there, any folder inside is considered as a potential MEU type/class.
- then in the application folder AAASeed search for a AAA_PROTO folder and, again, any folder inside is considered as a potential MEU type/class.
- this process is repeated one folder up until we reach the start folder (but including it) or the top of the file system.
- every time a **MEU_DIR** is open we also check for a AAA_PROTO on its folder.

for every of these potential **MEU** prototype folder we check the presence of a `fx.aaa_layers_all` (correspond to an AAASeed c_layers) or a `default.layerss_param` (correspond to a c_module). if one of thses file exist, we try to load it and use it to define a **MEU** prototype.

## isolated prototype
These prototype are called **isolated prototype** because they have their own folder, layers or module, lua script, data ...
When AAASeed try to load a **MEU** and don't find a corresponding prototype (using the lowercase meu_type of the name) it assime that the instance is a prototype, we call these **instance prototype**. 

```lua
-- m being a MEU
local b = m:is_proto()	-- Returns boolean
local b = m:is_proto_and_isolated()	-- Returns boolean
```

## proto init
## finding

<!---
# Prototype / class
In AAASeed with the lua language we deal with object oriented programming using metamethods (you could say also metatables).
You, a user, don't need to know all the details of this: here is what you need to need to know.











## proto init
## loading prototype
## finding
## isolated prototype





## MEU Reference 
MEU Reference (of type Ref) reference another MEU, this is a way to reuse a MEU in the rendering chain without duplicating it.  
This happens using the MEU name. The instance part of the name define which MEU is referenced.
For example MEU Ref_Displace_1 have an instance name Displace_1 and will refrrence a MEU Displace_1.  
The process used to find the referenced MEU is the same than for the cached version for the MEU:
search at the same level then below in the hierarchy and finally from the top. 
## APP_GP, GP

## MUS
-->

