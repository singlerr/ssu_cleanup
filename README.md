## 🛠️ Project #1: SSU-Cleanup

### 🌟 Overview

**SSU-Cleanup** is a file management and organization utility for Linux systems. Its core function is to organize files within a specified directory based on their **extensions**.

The project aims to:
* Understand **shell principles** by implementing new commands using system functions.
* Adapt to **standard and file I/O** by using system data structures, system calls, and libraries.
* Improve system programming skills by implementing the directory structure using a **linked list**.

| Category | Details |
| :--- | :--- |
| **Program Name** | `ssu_cleanup` |
| **Environment** | Linux system |
| **Scope** | Sub-paths under the user's home directory (`/home/user_id`) |
| **Prompt** | `학번)` (e.g., `20230000)`) |
| **Constraint** | **ABSOLUTELY NO USE of `system()` function** (results in 0 points) |

***

### 💻 Built-in Commands

The `ssu_cleanup` program includes four built-in commands:

#### 1. `tree`

Displays the directory and file structure recursively in a tree format.

* **Usage:** `tree (DIR_PATH) [OPTION]...`
* **Arguments:**
    * `<DIR_PATH>`: The starting directory path (supports relative and absolute paths).
    * `[OPTION]`: Can be `-s`, `-p`, or omitted. Multiple options can be used simultaneously.
* **Options:**
    * `(none)`: Basic recursive tree display. Directory names end with a slash (`/`).
    * `-s`: Includes the **size** of each file and directory.
    * `-p`: Includes the **permissions** of each file and directory.
    * `-sp`: Includes both **permissions and size**.
* **Output:** Must display the total number of directories and files at the end.

#### 2. `arrange`

Organizes files within a directory based on their extensions. The organized files are **copied** to an extension-specific subdirectory (e.g., `txt/`, `c/`) inside the result directory.

* **Usage:** `arrange (DIR_PATH) [OPTION]`
* **Arguments:**
    * `<DIR_PATH>`: The directory to be organized (supports relative and absolute paths).
    * `[OPTION]`: Optional. Includes `-d`, `-t`, `-x`, `-e`.
* **Result Directory:** By default, it creates `<DIR_PATH>_arranged` in the current working directory.
* **Duplicate File Handling:** If files with the same name are found within the directory to be organized, a prompt must appear, offering the following choices:
    * `0. select [num]`
    * `1. diff [num1] [num2]`
    * `2. vi [num]`
    * `3. do not select`
* **Options:**
    * `-d <output_path>`: Specifies `<output_path>` as the destination directory for the organized files.
    * `-t <seconds>`: Only arranges files that were **modified more than `<seconds>` seconds ago**.
    * `-x <exclude_path1, ...>`: Arranges the directory **excluding** files inside the specified exclude directories.
    * `-e <extension1, ...>`: Arranges files that only have the **specified extensions**.

#### 3. `help`

Outputs the usage (description) of the program's built-in commands.

* **Usage:** `help [COMMAND]`

#### 4. `exit`

Terminates the currently running `ssu_cleanup` program.

* **Usage:** `exit`

***

### ⚠️ Exception Handling (Deductions)

| Command | Exception Condition | Deduction (Points) |
| :--- | :--- | :--- |
| **General** | Enter key only: Re-output prompt. | 2 |
| | Input other than built-in commands: Output `help` usage, then re-output prompt. | 3 |
| **`tree`** | Invalid path (non-existent) as first argument: Output `Usage`, re-output prompt. | 2 |
| | Path exceeds max length (4096B) or name exceeds max length (256B). | 2 |
| | Path is not a directory or is outside the user's home directory. | 2 |
| | Absolute path is outside `$HOME` (`~`): Output ` <input path> is outside the home directory`, re-output prompt. | 2 |
| | Invalid option as second argument: Output `Usage`, re-output prompt. | 2 |
| **`arrange`** | No first argument: Output `Usage`, re-output prompt. | 3 |
| | Path exceeds max length (4096B) or name exceeds max length (256B). | 2 |
| | Path is not a directory or is outside the user's home directory. | 2 |
| | Invalid option as second argument: Output `Usage`, re-output prompt. | 5 |

***

| :--- | :--- | :--- |
| March 16th, 11:59 PM | **+30 points** to total score | Implement **all features** (mandatory + non-mandatory) and submit the report. |
| March 23rd, 11:59 PM | **+10 points** to total score | Implement **all features** (mandatory + non-mandatory) and submit the report. |
