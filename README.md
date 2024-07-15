# EPAM Cplusplus Mentoring Intermediate Level

# 01. Multithreading in C++ [Optional]

## Materials

Before you start (or after) It is recommended to go through everything what modern C++ offers in multithreading [here](https://en.cppreference.com/w/cpp/thread).

Please review the [link](https://en.cppreference.com/w/cpp/thread).

## Hints before start:

- Start the project as a prove of concept.
- No need to process all possible errors. Just check the count of parameters so 'source' and 'destination' parameters are present and source exists. (start with POC - prove of concept)
- Safety hint - do not overwrite destination file by default - but it is up to you.
- If you enjoy process or wants to continue: Create complete functional tool for backup process. Just pick up technical requirements for your tool from "copy /?" (windows) or "cp -man" (linux)

## What should be done

Your task is to implement simple 2 threaded "copy" tool.

- Tool should be implemented as a console application.
- Tool should accept 2 parameters source filename and target filename.
- Copying logic should be organized with help of 2 threads.
- First thread should read the data from source file.
- Second thread should write the data to the target file.

# 02. Multiprocessing Communication (Shared Memory) [Optional]

## Materials

Read about [new](https://eel.is/c++draft/expr.new) for organize reading to shared memory. As well as writing.
Discover how to organize interprocess synchronization (windows, Linux, MacOS) [Windows](https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory), [Linux](https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c), [boost](https://www.boost.org/doc/libs/1_56_0/doc/html/interprocess/sharedmemorybetweenprocesses.html)

Сan be useful to read: [link](https://en.cppreference.com/w/cpp/memory/new/operator_delete)

## Task

Read description of Module: Multithreading in C++

Your task is to rework the copy tool in the next manner:
1. Your tool should now accept 3 parameters <source> <destination> <name of shared memory object>
2. When run your tool by script twice.
2.1. First exemplar of executed tools should act as a reader of source file
2.2. Second exemplar of executed tools should act as a writer of destination file
3. Use 'placement' new for organize reading to shared memory. As well as writing.
4. Try to use constructor/destructor of a class - to notify about capture/release memory after read/write operation completion
5. Discover how to organize interprocess synchronization (windows, Linux, MacOS)

## What should be done

- Subtask: 3-d, 4-th and etc. exemplars of the application executed by the same script should just finish their work if <source> <destination> are the same, but should act as source and target if filenames differs
- Subtask: Implement timeout counting (in case if you have odd number of tools executed)

After completing task be ready to answer:
1. What kind of memory segments usual application has
2. What is the default size of the stack segment on the platform you have chosen to implement your tool
3. What is natural alignment

# 03. Exception Handling

# 04. Networking

# 05. Encryption

# 06. Static Polymorphism

# 07. Document/View Architecture [Optional]

# 08. \<algorithm>, \<chrono> and \<random>
