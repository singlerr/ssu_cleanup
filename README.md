## 🛠️ Project #1: SSU-Cleanup

### 🌟 Overview

**SSU-Cleanup** is a file management and organization utility for Linux systems. [cite_start]Its core function is to organize files within a specified directory based on their **extensions**[cite: 3].

The project aims to:
* [cite_start]Understand **shell principles** by implementing new commands using system functions[cite: 5].
* [cite_start]Adapt to **standard and file I/O** by using system data structures, system calls, and libraries[cite: 5].
* [cite_start]Improve system programming skills by implementing the directory structure using a **linked list**[cite: 5].

| Category | Details |
| :--- | :--- |
| **Program Name** | [cite_start]`ssu_cleanup` [cite: 42] |
| **Environment** | [cite_start]Linux system [cite: 3] |
| **Scope** | [cite_start]Sub-paths under the user's home directory (`/home/user_id`) [cite: 38] |
| **Prompt** | [cite_start]`학번)` (e.g., `20230000)`) [cite: 48, 52] |
| **Constraint** | [cite_start]**ABSOLUTELY NO USE of `system()` function** (results in 0 points) [cite: 40] |

***

### 💻 Built-in Commands

[cite_start]The `ssu_cleanup` program includes four built-in commands [cite: 37, 44-45]:

#### 1. `tree`

[cite_start]Displays the directory and file structure recursively in a tree format[cite: 75].

* [cite_start]**Usage:** `tree (DIR_PATH) [OPTION]...` [cite: 74]
* **Arguments:**
    * [cite_start]`<DIR_PATH>`: The starting directory path (supports relative and absolute paths)[cite: 77].
    * `[OPTION]`: Can be `-s`, `-p`, or omitted. [cite_start]Multiple options can be used simultaneously[cite: 78].
* **Options:**
    * [cite_start]`(none)`: Basic recursive tree display[cite: 58]. [cite_start]Directory names end with a slash (`/`)[cite: 83].
    * [cite_start]`-s`: Includes the **size** of each file and directory[cite: 59, 121].
    * [cite_start]`-p`: Includes the **permissions** of each file and directory [cite: 60-61, 142].
    * [cite_start]`-sp`: Includes both **permissions and size**[cite: 163].
* [cite_start]**Output:** Must display the total number of directories and files at the end[cite: 81].

#### 2. `arrange`

[cite_start]Organizes files within a directory based on their extensions[cite: 193]. [cite_start]The organized files are **copied** to an extension-specific subdirectory (e.g., `txt/`, `c/`) inside the result directory[cite: 199].

* [cite_start]**Usage:** `arrange (DIR_PATH) [OPTION]` [cite: 192]
* **Arguments:**
    * [cite_start]`<DIR_PATH>`: The directory to be organized (supports relative and absolute paths)[cite: 195].
    * `[OPTION]`: Optional. [cite_start]Includes `-d`, `-t`, `-x`, `-e`[cite: 196].
* [cite_start]**Result Directory:** By default, it creates `<DIR_PATH>_arranged` in the current working directory[cite: 193, 198].
* [cite_start]**Duplicate File Handling:** If files with the same name are found within the directory to be organized, a prompt must appear, offering the following choices [cite: 200, 233-237]:
    * `0. select [num]`
    * `1. diff [num1] [num2]`
    * `2. vi [num]`
    * `3. do not select`
* **Options:**
    * [cite_start]`-d <output_path>`: Specifies `<output_path>` as the destination directory for the organized files [cite: 64-65, 247].
    * [cite_start]`-t <seconds>`: Only arranges files that were **modified more than `<seconds>` seconds ago**[cite: 66, 318].
    * [cite_start]`-x <exclude_path1, ...>`: Arranges the directory **excluding** files inside the specified exclude directories[cite: 67, 319].
    * [cite_start]`-e <extension1, ...>`: Arranges files that only have the **specified extensions** [cite: 68-70, 282, 320, 322].

#### 3. `help`

[cite_start]Outputs the usage (description) of the program's built-in commands [cite: 302, 307-323].

* [cite_start]**Usage:** `help [COMMAND]` [cite: 321]

#### 4. `exit`

[cite_start]Terminates the currently running `ssu_cleanup` program[cite: 326, 328].

* [cite_start]**Usage:** `exit` [cite: 325]

***

### ⚠️ Exception Handling (Deductions)

| Command | Exception Condition | Deduction (Points) |
| :--- | :--- | :--- |
| **General** | [cite_start]Enter key only: Re-output prompt[cite: 72]. | 2 |
| | [cite_start]Input other than built-in commands: Output `help` usage, then re-output prompt[cite: 72]. | 3 |
| **`tree`** | [cite_start]Invalid path (non-existent) as first argument: Output `Usage`, re-output prompt[cite: 186]. | 2 |
| | [cite_start]Path exceeds max length (4096B) or name exceeds max length (256B)[cite: 187]. | 2 |
| | [cite_start]Path is not a directory or is outside the user's home directory[cite: 188]. | 2 |
| | [cite_start]Absolute path is outside `$HOME` (`~`): Output ` <input path> is outside the home directory`, re-output prompt[cite: 189]. | 2 |
| | [cite_start]Invalid option as second argument: Output `Usage`, re-output prompt[cite: 190]. | 2 |
| **`arrange`** | [cite_start]No first argument: Output `Usage`, re-output prompt[cite: 296]. | 3 |
| | [cite_start]Path exceeds max length (4096B) or name exceeds max length (256B)[cite: 297]. | 2 |
| | [cite_start]Path is not a directory or is outside the user's home directory[cite: 298]. | 2 |
| | [cite_start]Invalid option as second argument: Output `Usage`, re-output prompt[cite: 299]. | 5 |

***
