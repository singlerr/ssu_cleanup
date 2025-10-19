## 🛠️ Project #1: SSU-Cleanup

### 🌟 Overview

[cite_start]SSU-Cleanup is a program for managing and organizing files in a Linux system based on their extensions[cite: 1, 3]. [cite_start]The objective is to understand the shell's principles by implementing new commands using system functions [cite: 5][cite_start], and to improve system programming design and application skills by utilizing various system data structures, system calls, and libraries for standard and file input/output[cite: 5]. [cite_start]The directory structure should be implemented as a linked list[cite: 5].

| Category | Description |
| :--- | :--- |
| **Program Name** | [cite_start]`ssu_cleanup` [cite: 42] |
| **Execution Environment** | [cite_start]Linux system [cite: 3] |
| **Organizable Paths** | [cite_start]Sub-paths under the user's home directory (`/home/user_id`) [cite: 38] |
| **Execution Prompt** | [cite_start]`학번)` (e.g., `20230000)`) [cite: 48, 52] |
| **Constraint** | [cite_start]Absolute prohibition of using `system()` function in the entire program (results in 0 points) [cite: 40] |

---

### 💻 Built-in Commands

[cite_start]The `ssu_cleanup` program consists of the following four built-in commands [cite: 37, 44-45]:

#### 1. `tree`

[cite_start]A command to display directories and files in a tree structure[cite: 44, 75].

* [cite_start]**Usage:** `tree (DIR_PATH) [OPTION]...` [cite: 56, 74]
* **Arguments:**
    * [cite_start]`<DIR_PATH>`: The path of the directory to be printed, can be relative or absolute[cite: 77].
    * [cite_start]`[OPTION]`: Can be `-s`, `-p`, or omitted, and can be used simultaneously [cite: 56, 59-60, 78].
* **Options:**
    * [cite_start]`(none)`: Display the directory structure recursively if `<DIR_PATH>` is a directory[cite: 58].
    * [cite_start]`-s`: Display the directory structure recursively, including the **size of each file**[cite: 59, 121].
    * [cite_start]`-p`: Display the directory structure recursively, including the **permissions** of each directory and file [cite: 60-61, 142].

#### 2. `arrange`

[cite_start]A command to organize files within a specified directory based on file extensions[cite: 44, 193]. [cite_start]The result is saved in `<DIR_PATH>_arranged` under the current working directory[cite: 193].

* [cite_start]**Usage:** `arrange (DIR_PATH) [OPTION]` [cite: 62, 192]
* **Arguments:**
    * [cite_start]`<DIR_PATH>`: The path of the directory to be organized, can be relative or absolute[cite: 195].
    * [cite_start]`[OPTION]`: Can be `-d`, `-t`, `-x`, `-e`, and all are optional[cite: 196].
* **Organization Result:**
    * [cite_start]The `<DIR_PATH>_arranged` directory is created in the current working directory if it does not exist[cite: 198].
    * [cite_start]Extension-specific directories (e.g., `txt`, `c`, `cpp`) are created inside the `_arranged` directory, and files with the corresponding extension are **copied** into them[cite: 199].
    * [cite_start]If files with the same name are found in the directory being organized, the user can choose to select a file, `diff`, `vi`, or not to select/arrange it [cite: 200, 233-237].
* **Options:**
    * [cite_start]`(none)`: Basic arrangement[cite: 63, 315].
    * [cite_start]`-d <output_path>`: Specify the output directory (`<output_path>`) where `<DIR_PATH>` will be arranged [cite: 64-65, 247, 316-317].
    * [cite_start]`-t <seconds>`: Only arrange files that were modified **more than `<seconds>` seconds ago**[cite: 66, 262, 318]. (Note: Example 2-3 says "last modified time is **within** (seconds) [cite_start]seconds ago" in the caption, but the description says "modified **more than** (seconds) seconds ago"[cite: 262]. The description is likely the requirement, but the example's context suggests the opposite. [cite_start]The text "Only arrange files that were modified more than (seconds) seconds ago" is used in the description [cite: 262, 318]).
    * [cite_start]`-x <exclude_path1, exclude_path2, ...>`: Arrange the directory **except for the files inside the specified exclude directories**[cite: 67, 272, 319].
    * [cite_start]`-e <extension1, extension2, ...>`: Arrange the directory only with the **specified extensions** [cite: 68-70, 282, 320, 322].

#### 3. `help`

[cite_start]A command to print the usage and description of the program's built-in commands[cite: 44, 302, 304].

* [cite_start]**Usage:** `help [COMMAND]` [cite: 69, 321]

#### 4. `exit`

[cite_start]A command to terminate the currently running `ssu_cleanup` program[cite: 45, 326, 328].

* [cite_start]**Usage:** `exit` [cite: 69, 323, 325]

---

### ⚠️ Exception Handling (Non-Mandatory for Implementation)

[cite_start]Failing to implement these results in deductions, but they are not part of the "Mandatory Implementation"[cite: 71, 185, 294].

| Built-in Command | Exception Handling Description | Deduction (Points) |
| :--- | :--- | :--- |
| **Common** | [cite_start]Enter only: Re-output the prompt[cite: 72]. | 2 |
| | [cite_start]Input of a command other than a built-in command: Output `help` usage and re-output the prompt[cite: 72]. | 3 |
| **`tree`** | [cite_start]Invalid path (non-existent directory) as the first argument: Output `Usage` and re-output the prompt[cite: 186]. | 2 |
| | [cite_start]Path (absolute or relative) exceeds length limit (4096 Bytes), or file/directory name length exceeds limit (256 Bytes): Error handling[cite: 187]. | 2 |
| | [cite_start]Path is not a directory, or the path (absolute or relative) is outside the user's home directory: Error handling and re-output the prompt[cite: 188]. | 2 |
| | [cite_start]Absolute path is outside the user's home directory: Standard output of `<input path> is outside the home directory` and re-output the prompt[cite: 189]. | 2 |
| | [cite_start]Invalid option as the second argument: Output `Usage` and re-output the prompt[cite: 190]. | 2 |
| **`arrange`** | [cite_start]No first argument: Output `Usage` and re-output the prompt[cite: 296]. | 3 |
| | [cite_start]Path (absolute or relative) exceeds length limit (4096 Bytes), or file/directory name length exceeds limit (256 Bytes): Error handling[cite: 297]. | 2 |
| | [cite_start]Path is not a directory, or the path (absolute or relative) is outside the user's home directory: Error handling and re-output the prompt[cite: 298]. | 2 |
| | [cite_start]Invalid option as the second argument: Output `Usage` and re-output the prompt[cite: 299]. | 5 |

---

### 💯 Grading Criteria

[cite_start]The completeness of the project is determined by the implementation of the specified "**Mandatory Implementation**"[cite: 23, 399].

| Item | Score (Out of 100) | Mandatory Implementation | Options/Other (Deduction for non-implementation) |
| :--- | :--- | :--- | :--- |
| **Implementation Report** (`P<project_num>.hwp`) | 15 points | ❌ | [cite_start]Overview (1), Functionality (1), Detailed Design (10), Execution Result (3) [cite: 20] |
| **Source Code Comments** | 5 points | ❌ | - |
| **Execution Status** (Final conversion to 80 points) | 80 points | - | - |
| **1. `ssu_cleanup`** | 10 points | [cite_start]⭕ [cite: 400] | - |
| **2. `tree`** | 25 points | [cite_start]⭕ (Base) [cite: 401] | [cite_start]`-s` option (5 points) [cite: 402][cite_start], `-p` option (5 points) [cite: 403] |
| **3. `arrange`** | 50 points | [cite_start]❌ (Base is not mandatory) [cite: 404] | [cite_start]Base function (20 points for non-implementation) [cite: 404][cite_start], `-d` (5 points) [cite: 405][cite_start], `-t` (5 points) [cite: 406][cite_start], `-e` (10 points) [cite: 407][cite_start], `-x` (10 points) [cite: 408] |
| **4. `help`** | 5 points | [cite_start]⭕ [cite: 409] | - |
| **5. `exit`** | 5 points | [cite_start]⭕ [cite: 410] | - |
| **6. `Makefile` Creation** | 5 points | [cite_start]⭕ [cite: 411] | [cite_start]Use of macro is not mandatory [cite: 411] |
