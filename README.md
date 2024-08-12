# EPAM Cplusplus Mentoring Intermediate Level

# 01. Multithreading in C++ [Optional]

## Materials

Before you start (or after) It is recommended to go through everything what modern C++ offers in multithreading [here](https://en.cppreference.com/w/cpp/thread).

Please review the [link](https://en.cppreference.com/w/cpp/thread).

## Hints before start:

- Start the project as a prove of concept.
- No need to process all possible errors. Just check the count of parameters so 'source' and 'destination' parameters are present and source exists. (start with POC - prove of concept).
- Safety hint - do not overwrite destination file by default - but it is up to you.
- If you enjoy process or wants to continue: Create complete functional tool for backup process. Just pick up technical requirements for your tool from "copy /?" (windows) or "cp -man" (linux).

## What should be done

Your task is to implement simple 2 threaded "copy" tool.

- Tool should be implemented as a console application.
- Tool should accept 2 parameters source filename and target filename.
- Copying logic should be organized with help of 2 threads.
- First thread should read the data from source file.
- Second thread should write the data to the target file.

# 02. Multiprocessing Communication (Shared Memory) [Optional]

## Materials

Read about [new](https://eel.is/c++draft/expr.new) for organize reading to shared memory. As well as writing.
Discover how to organize interprocess synchronization (windows, Linux, MacOS) [Windows](https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory), [Linux](https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c), [boost](https://www.boost.org/doc/libs/1_56_0/doc/html/interprocess/sharedmemorybetweenprocesses.html).

Сan be useful to read: [link](https://en.cppreference.com/w/cpp/memory/new/operator_delete).

## Task

Read description of Module: Multithreading in C++

Your task is to rework the copy tool in the next manner:

1. Your tool should now accept 3 parameters ```<source> <destination> <name of shared memory object>```.
2. When run your tool by script twice.
2.1. First exemplar of executed tools should act as a reader of source file.
2.2. Second exemplar of executed tools should act as a writer of destination file.
3. Use 'placement' new for organize reading to shared memory. As well as writing.
4. Try to use constructor/destructor of a class - to notify about capture/release memory after read/write operation completion.
5. Discover how to organize interprocess synchronization (windows, Linux, MacOS).

## What should be done

- Subtask: 3-d, 4-th and etc. exemplars of the application executed by the same script should just finish their work if <source> <destination> are the same, but should act as source and target if filenames differs.
- Subtask: Implement timeout counting (in case if you have odd number of tools executed).

After completing task be ready to answer:

1. What kind of memory segments usual application has.
2. What is the default size of the stack segment on the platform you have chosen to implement your tool.
3. What is natural alignment.

# 03. Exception Handling

## Materials

Find yourself and read at least 3 different sources about exception handling in C++.
Investigate about levels of exception safety guaranties in C++.
Read from link below as well as all nearby links at the same level of the documentation: [link](https://docs.microsoft.com/en-us/cpp/cpp/exceptions-and-stack-unwinding-in-cpp?view=msvc-170), [link](https://eel.is/c++draft/except.throw).

## Task

Investigate whether any destructor of the objects in your logic will be executed in case if:

- You have appropriate catch keyword.
- You do not have any catch.
- Share your observations with mentor.

NOTE: Take care about closing both exemplars of application if one of them closed after exception. It is up to you how to organize this process.

## What should be done

- Implement own terminate function to close at any circumstances shared memory handle as well as opened for reading/writing file.
- Add throw any exception code after complete of first memory block reading/writing (take care about "to close" another process).
- Inject some classes in logic for logging purposes (constructor + destructor).

# 04. Networking

## Materials

[Boost ASIO to work with networking](https://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio.html), [Good intro for read](https://theboostcpplibraries.com/boost.asio).

## What should be done

Read description of task Multithreading in C++.
Read description of task Multiprocessing communication in C++.

Your task is to rework the copy tool in the next manner:

1. The same application could act as server and as client (depends on run-parameter).
2. When it runs as a server - it accumulates all files which could be sent to it from client(s). It is up to you where to accumulate all files from clients - but your server should not overwrite files from different clients if they have the same name.
3. When it runs as client it sends to server files - accept file names as command line parameters.

NOTE: Now you should take care about processing of messages communication between client and servers. take care about attributes of files together with names.
NOTE: It is up to you how to organize file distribution on server side (2 files with the same name from different clients cannot be intersected/overlapped/merged).
NOTE: This task is basis for the next one - so care about S.O.L.I.D.

# 05. Encryption

## Materials

You can use [library](https://www.cryptopp.com/) for encryption of the data. Though feel free to find any other library yourself and use it for encryption/decryption.

## What should be done

Rework the code from the module Networking.
Now client application should encrypt file data after reading and before sending.
Server application should decrypt acquired data before writing.

It is up to you to decide what encryption algorithm to use.
It is up to you how to generate encryption keys/passwords
Ensure that source and target files are the same.

NOTE: Application will be elaborated further.

# 06. Static Polymorphism

## Materials

Review the [material](https://en.cppreference.com/w/cpp/language/crtp).

## What should be done

Split application from module Encryption to two: Client and Server.
Define template using of Curiously recurring Template pattern which obeys inner class type to has 3 non-virtual member methods:

- ProcessData.
- NotifyComplete.
- WaitNextData.

Both applications (client & server) should use this template.

Client must use (at least) two specifications for Inner type:

- One for reader.
- One for encryption.

Server side must have at least two specification:

- One for decryption.
- One for writer.

NOTE: Since the logic of data processing becoming more and more complex - consider creation of message flow for data processing. Use the same (templated) approach to organize standardized work with messages.

# 07. Document/View Architecture [Optional]

## What should be done

On The basis of Module Static Polymorphism organize client server application with next demands:

1. Server should be able to link 2 or more different clients.
2. Each Client should be able to represent own file system and share it with all other clients through server (think about generation of unique name for each client).
3. Each Client should use the same graphical representation of file system for self and for all other clients. (Use "Total commander" as UI example - left & right panels).
4. Each Client should be able to browse through filesystem(s) of each client connected to server at the moment and initiate copy logic (through server) - to copy file from any client to another.
5. if "client 1" initiates coping from "client 2" to "client 3", then "client 1" (and only "client 1") should observe status of copying, but do not participate in the copying process.

EXTRA:

6. Implement copy of folder(s) and set of files
7. Copying (point 5) should continue even If "client 1" disconnects.
8. Status of Copying (point 5,7) should be presented to the client 1, If "client 1" reconnects. (for testing use slowdowns for copying of file).

HINTS:

- Start with defining of the set of messages between clients.
- Use UML to define server & client logic.
- Server should not hold any kind of data (just do work as a proxy).
- it is up to you to use encryption.
- It is your decision about UI representation - QT, MFC, WinAPI, GTK+.

NOTE: This task is complex separate application - it is not mandatory for the course completion.

# 08. \<algorithm>, \<chrono> and \<random>

## Materials

Review the materials:

- [Numeric/Random](https://en.cppreference.com/w/cpp/numeric/random).
- [Quicksort](https://en.wikipedia.org/wiki/Quicksort).
- [Chrono/Time_point](https://en.cppreference.com/w/cpp/chrono/time_point).
- [Youtube video](https://www.youtube.com/watch?v=2olsGf6JIkU).

## What should be done

1. With help of [Numeric/Random](https://en.cppreference.com/w/cpp/numeric/random) create a logic to generate set of unique double values in some range.
2. Investigate qsort algorithm from wiki [Quicksort](https://en.wikipedia.org/wiki/Quicksort).
3. Implement own version for qsort and apply it for generated from point 1. set.
4. Implement logic to measure work time of your qsort algorithm in microseconds (1E-6 sec.) [Chrono/Time_point](https://en.cppreference.com/w/cpp/chrono/time_point).
5. Compare your implementation time work with standard qsort.
6. Optimize your implementation of qsort to be multithreaded but to work only with count of threads no more than number of cores in the PC where your application works.
7. Compare timing for all versions. (provide diagrams for standard qsort, your qsort, your multithreaded qsort, depends on the amount of the input data: 2 values, 10 values, 1000 values, 1000000 values)
8. Provide time measurements for work of your algorithm on already sorted data and compare this with timing demonstrated by standard implementation.
