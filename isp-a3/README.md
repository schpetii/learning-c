# Assignment 3 - Stringtango

## Introduction

In Assignment 3 (A3) of the ISP practicals (KU), you will implement an editor for simple string operations.
Please make sure that you follow the assignment description exactly, because even slight differences can cause
testcases to fail.
This document describes the structure of the assignment and the expected functionality of the program.

## Learning Goals

Memory Management/Heap

## Specifications

There are some general specifications that must be followed for every assignment in this course. A list of these and which point deductions
may result from violating them can be found here:
[Assessment of Assignments - Deductions](https://www.notion.so/coding-tugraz/Assessment-of-Assignments-How-do-you-get-your-points-d7cca6cc89a344e38ad74dd3ccb73cb3?pvs=4#e3203e50b1fc4ba787bdbf5fb46262ae).
Please follow these specifications to avoid point deductions. As **memory management** is a central learning goal of 
this assignment, pay close attention to this section within the specifications.

Additionally, the following specifications must be followed for this assignment (A3) as well:

* **Attention:** `malloc()` (or `calloc()`), `realloc()` and `free()` **must** be used. All strings **must** be stored on the heap. Failure to do so may result in a deduction of **up to 25%**.
* **Attention:** Your program **must** be able to handle inputs of arbitrary length. Failure to do so may result in a deduction of **up to 25%**.
* **Attention:** Strings **must** only use as much memory as needed to store their content. Temporary extra memory during command execution is allowed, but after the command finishes (successfully or with an error), no additional memory should remain allocated. If strings use more memory than needed, this will result in a deduction of **up to 25%**.
* **Attention:** The function `qsort()` **must not** be used. Using it will result in a deduction of **up to 25%**.


## Grading

Assignment 3 is worth **18 points**.

Information on how your assignment submission will be assessed can be found here: [Assessment of Assignments](https://www.notion.so/coding-tugraz/Assessment-of-Assignments-How-do-you-get-your-points-d7cca6cc89a344e38ad74dd3ccb73cb3).
Please read this carefully, as it also contains guidelines on what your final code should look like (Coding Standard, Style Guide).

Here you can find an overview of how your total grade for the KU will be calculated: [Procedure and Grading](https://www.notion.so/coding-tugraz/Procedure-and-Grading-59975fee4a9c4047867772cd20caf73d)

## Submission

- Deliverables: `a3.c`
- Push to your repository
- **Deadline: 10.12.2025, 18:00 (Austrian time)**

## Weekly Exercises

There are **Weekly Exercises** associated with this assignment available on [TeachCenter](https://tc.tugraz.at/main/course/section.php?id=72261).
These are simple exercises designed to help you prepare for the assignment, and **must** be solved at least partially to avoid deductions.

- **Attention**: At least 50% of the Weekly Exercises for A3 must be solved before the A3 deadline. Failure to meet this requirement will result in a **25% point deduction on A3**.

## Assignment Description

This program implements a basic interactive text editor that allows the user to modify a single text stored on the heap using a small set of predefined commands.

### General Notes

The notes given in this section are important for the entire assignment description:

- `\n` should not be printed as separate characters, instead they represent the newline character.
- Note the leading and trailing spaces in the text fields! You can make them visible by selecting the text field.
- Words in angle brackets (such as `<TEXT>`) should be replaced by calculated or given data. They are **not** part of the output itself! For a better understanding, you can look at the [example outputs](#example-outputs) at the very bottom of this document.
- <code>&nbsp;>&nbsp;</code> (command prompt, always printed with a leading and trailing space) in the output indicates that the program should wait for user input at this point. Execution should only continue after the user submits their input with `ENTER`.
- Make sure that your program can handle inputs of arbitrary length including empty inputs (see also the 
  specifications).
- If the user provides invalid input during the execution of a command, the command must not be performed. The corresponding error message should be printed, and the program should return directly to the main command prompt, where the user can select another command.
- Any time the program waits for user input, `quit` should be a valid input. If the user enters `quit`, the program 
  should terminate with the return value `0` without printing anything else. Take care to free all previously allocated 
  memory!
- Whitespace characters are always part of the input (<code>&nbsp;quit&nbsp;</code> is not the same as `quit`).
- The user input (including commands) is **case-sensitive**, meaning `quit` is a valid input, but `QUIT` or `qUiT` are not.
- Take care with your memory management! Whenever you allocate memory on the heap, you must check that the allocation was successful. If the program runs out of memory, you should free all 
  memory previously allocated on the heap, print the correct error message and terminate the program with the 
  corresponding return value (see [Return Values and Error Messages](#return-values-and-error-messages)).
- When the length of a string changes (either grows or shrinks),
  make sure that the memory allocated for it is updated accordingly. Free any memory that is no longer needed to avoid wasting resources.

## Program Sequence

The program runs an interactive loop that operates on a single text stored on the heap. First, it prints a welcome message and asks the user to enter an initial text. 
Then it repeatedly offers a small set of commands to modify that text. 
The loop only ends when the user chooses the "Quit" command or types `quit` at any input prompt. 
Upon quitting, free all allocated memory and exit with the correct return value (see [Return Values and Error Messages](#return-values-and-error-messages)).

<details><summary><h3>Welcome Message</h3></summary>

At the beginning of the program, the user is welcomed with the following message:

```
\n
Welcome to the\n
Stringtango Text Editor!\n
\n
```

</details>

<details><summary><h3>Initial Text Input</h3></summary>

After printing the welcome message, the user is asked to input the initial text with the following prompt:

```
Enter a text:\n
 > 
```

At this point, the user must enter the text they want to edit. The text can be of arbitrary length (including
empty) and **must** be stored on the heap.

> **Note:** Non-printable characters will not be tested and do not have to be handled.

> **Attention:** Keep in mind that all whitespace characters are kept as part of the input.

</details>

<details><summary><h3>Command Loop</h3></summary>

After the initial text input, the user can enter commands to edit the text. 
The user is prompted to select the next command by entering its abbreviation:

```
\n
Choose a command:\n
 a: append text\n
 r: search and replace\n
 s: split and sort\n
 u: unique\n
 q: quit\n
\n
 > 
```

> **Note:** At this prompt, both `q` and `quit` are valid inputs to terminate the program.

On invalid command input, print the specified error 
(see [Return Values and Error Messages](#return-values-and-error-messages)), then re-prompt the user to enter a command. Repeat this process until a valid input is provided.

When the user selects a command, that command is executed as described in the corresponding section below. After a command has been executed successfully, the updated text is printed in the output format specified further below, and the program then returns to the main command prompt.

<details><summary><h4>Append Text</h4></summary>

When the user selects this command by entering `a`, the program prints the following prompt and appends a newly entered string to the end of the current text:

```
\n
Please enter the string that should be appended:\n
 > 
```

The new string must not be empty (i.e., length zero). Otherwise, an error message should be printed
(see [Return Values and Error Messages](#return-values-and-error-messages)).

> **Note:** A string that contains only whitespace characters (one or multiple) is **not** considered empty.

> **Example:** \
> Current text: `hello` \
> New string to append: <code>&nbsp;world</code> \
> Text after appending: `hello world`
</details>

<details><summary><h4>Search and Replace</h4></summary>

When the user selects this command by entering `r`, a search-and-replace operation is performed in two steps.

1. Prompt for the substring to find in the current text:

```
\n
Please enter the substring to search for:\n
 > 
```

An error message should be printed if the string is empty or if the substring cannot be found in the current text
(see [Return Values and Error Messages](#return-values-and-error-messages)).

2. If a valid substring has been provided, prompt the user for the replacement string:

```
\n
Please enter the new substring:\n
 > 
```

Replace all occurrences of the searched substring in the current text with the new replacement string.

> **Example:** \
> Current text: `hello world` \
> Substring to find: `world` \
> Replacement string: `everyone` \
> Text after replacement: `hello everyone`
</details>

<details><summary><h4>Split and Sort</h4></summary>

When the user selects this command by entering `s`, the program splits the current text in a sequence of words and sorts them in ascending lexicographical order.
After sorting, the words are joined back into a single string with a single space separating each word.

> **Note:** A _word_ is defined as a sequence of non-whitespace characters separated by whitespace.

> **Note:** In this context _lexicographical order_ refers to alphabetical ordering based on the ASCII values of the characters.
> For example, digits come before uppercase letters, which in turn come before lowercase letters.

> **Note:** If one word is the _prefix of another word_, the shorter word should come before the longer word.
> For example, <code>dishwasher dish</code> should be sorted to <code>dish dishwasher</code>.

> **Attention:** When splitting the text into words, ignore leading, trailing, and multiple consecutive whitespace characters. As a result, words must not contain leading or trailing whitespace, and no empty words should be created.

The sorted words are then joined back into a single string with exactly one space between each word. The resulting text must not contain any leading or trailing whitespace, and it is stored as the new text.

If the text is empty or contains only whitespace characters, it remains unchanged, and an error message is printed
(see [Return Values and Error Messages](#return-values-and-error-messages)).

> **Example:** \
> Current text:  <code>&nbsp;banana orange&nbsp;&nbsp;apple</code> \
> Text after splitting, sorting, and joining: `apple banana orange`
</details>

<details><summary><h4>Unique</h4></summary>

When the user selects this command by entering `u`, the program processes the current text to remove duplicate words,
keeping only the first occurrence of each unique word.

> **Note:** A _word_ is defined as a sequence of non-whitespace characters separated by whitespace.

> **Note:** Uniqueness is determined case-insensitively. For example, `this`, `This`, and `THIS` are all considered the same word.

> **Attention:** When splitting the text into words, ignore leading, trailing, and multiple consecutive whitespace characters. As a result, words must not contain leading or trailing whitespace, and no empty words should be created.

The remaining words are then joined back together, in their original order,
into a single string, with exactly one space separating each word.
The resulting string must not contain any leading or trailing whitespace, and it is stored as the new text.

If the text does not contain any duplicate words (i.e., all words are already unique or the text contains only whitespace characters), it remains unchanged, and an error message is printed
(see [Return Values and Error Messages](#return-values-and-error-messages)).

> **Example:** \
> Current text:  <code>&nbsp;banana apple&nbsp;&nbsp;Banana</code> \
> Text after splitting, deleting duplicates, and joining: `banana apple`
</details>

<details><summary><h4>Quit</h4></summary>

The "Quit" command is selected by entering `q`. The only difference between this command and typing `quit` at any prompt is that the "Quit" command 
prints the current text before terminating the program, whereas
typing `quit` immediately terminates the program without printing anything else.

</details>

<hr/>

After executing a command successfully, the program should print the updated text in the following format:

```
\n
Current text:\n
<TEXT>\n
```

where `<TEXT>` is the updated text after executing the command.

Afterwards, the program prompts the user to choose the next command and continues this loop until the user quits.

> **Attention:** You should not print the current text if the command resulted in an error. Instead, print the error message and re-prompt for a command.

</details>

### Return Values and Error Messages

The program has to react to errors during the execution and print the corresponding error messages and/or return the 
correct return values. 
You can find all error messages that should be printed and all return values in the table below.

Only one error should be printed at once. If multiple errors occur at the same time, only the highest message in the below table should be printed.

| Return Value | Error Message                            | Meaning                                                                                                                        |
|--------------|------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------|
| 0            | -                                        | The program terminated successfully (user entered `quit` or used the "Quit" command)                                           |
| 1            | `[ERROR] Memory allocation failed!\n`    | The program was not able to allocate new memory                                                                                |
|              | `[ERROR] Command unknown!\n`             | The user entered an invalid command                                                                                            |
|              | `[ERROR] String can't be empty!\n`       | The string entered for the "Append Text" or "Search and Replace" command was empty                                             |
|              | `[ERROR] Substring not found!\n`         | The string entered for the "Search and Replace" command was not part of the current text                                       |
|              | `[ERROR] No words to sort!\n`            | The current text was empty or contained only whitespace characters when executing the "Split and Sort" command                 |
|              | `[ERROR] All words are already unique\n` | The current text was empty, contained only whitespace characters or had no duplicate words when executing the "Unique" command |

## Example Outputs

<details>
<summary><h4>Example 1</h4></summary>

```

Welcome to the
Stringtango Text Editor!

Enter a text:
 > The weather today is very nice!

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > r

Please enter the substring to search for:
 > nice

Please enter the new substring:
 > bad

Current text:
The weather today is very bad!

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > a

Please enter the string that should be appended:
 >  Hopefully it is better tomorrow.

Current text:
The weather today is very bad! Hopefully it is better tomorrow.

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > s

Current text:
Hopefully The bad! better is is it today tomorrow. very weather

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > q

Current text:
Hopefully The bad! better is is it today tomorrow. very weather
```

</details>

<details>
<summary><h4>Example 2</h4></summary>

```

Welcome to the
Stringtango Text Editor!

Enter a text:
 > banana sugar apple sugar wasabi flour flour pear

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > a

Please enter the string that should be appended:
 > juice water

Current text:
banana sugar apple sugar wasabi flour flour pearjuice water

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > r

Please enter the substring to search for:
 > WATER
[ERROR] Substring not found!

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > water
[ERROR] Command unknown!

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > r

Please enter the substring to search for:
 > water

Please enter the new substring:
 > milk

Current text:
banana sugar apple sugar wasabi flour flour pearjuice milk

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > r

Please enter the substring to search for:
 > sugar

Please enter the new substring:
 > honey

Current text:
banana honey apple honey wasabi flour flour pearjuice milk

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > a

Please enter the string that should be appended:
 > QuiT

Current text:
banana honey apple honey wasabi flour flour pearjuice milkQuiT

Choose a command:
 a: append text
 r: search and replace
 s: split and sort
 u: unique
 q: quit

 > a

Please enter the string that should be appended:
 > quit

```
</details>