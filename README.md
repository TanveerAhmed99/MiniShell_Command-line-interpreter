
---

# MiniShell – Command Line Interpreter

**MiniShell** is a lightweight **command-line interpreter** written in C. It replicates core shell functionality such as executing commands, handling pipes and redirection, maintaining history, and managing signals. The project showcases process control, file descriptor management, and system programming concepts in C.

---

## ✨ Features

* **Command Execution**

  * Supports execution of system commands with arguments (e.g., `ls -l`, `cat file.txt`).

* **Built-in Commands**

  * `cd` – change directory
  * `pwd` – print working directory
  * `history` – display command history
  * `exit` – exit MiniShell

* **Command History**

  * Stores up to 100 previous commands
  * View with `history` command

* **Redirection**

  * Input redirection: `command < file`
  * Output redirection (overwrite): `command > file`
  * Output redirection (append): `command >> file`

* **Pipes (`|`)**

  * Supports single and multiple pipes (e.g., `ls -l | grep .c | wc -l`).

* **Multiple Commands**

  * Sequential execution: `command1 ; command2`
  * Conditional execution: `command1 && command2`

* **Signal Handling**

  * Graceful handling of **Ctrl+C** (`SIGINT`) without terminating the shell.

---

## 📂 Project Structure

```
Command_line_interpreter.c   # Source code for MiniShell
```

---

## 🚀 Compilation & Usage

### 1. Compile the source:

```bash
gcc Project1.c -o minishell
```

### 2. Run the shell:

```bash
./minishell
```

### 3. Example Session:

```bash
sad> pwd
/home/user/projects

sad> ls -l | grep .c > output.txt

sad> history
1 pwd
2 ls -l | grep .c > output.txt
```

---

## ⚙️ Technical Highlights

* **Process Management** – uses `fork()`, `execvp()`, and `wait()`
* **File Descriptors** – uses `dup2()` and `pipe()` for redirection & piping
* **Parsing Logic** – tokenizes input with `strtok()`
* **Signal Handling** – custom `SIGINT` handler for graceful interruption
* **Command History** – implements in-memory history tracking with `strdup()`

---

## 📚 Learning Outcomes

By building MiniShell, you gain hands-on experience with:

* System-level programming in C
* Process creation & synchronization
* File descriptor manipulation
* Command parsing/tokenization
* Designing a custom CLI

---

## 🔮 Future Enhancements

* Add support for background processes (`&`)
* Implement tab auto-completion
* Persist history across sessions
* Enhanced error handling & reporting

---

## 📝 License

This project is released under the **MIT License**.
You are free to use, modify, and distribute it for educational or personal purposes.

---

