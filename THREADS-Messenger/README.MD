# Messenger Project Solution

## CYBV 489 Principles of Operating Systems

`Anthony Tropeano` `Connor Stackhouse`

## Table of Contents

- [Overview](#overview)
- [Contributors](#contributors)
- [Usage](#usage)
- [Solution](#solution)

## Overview

Inter-Process Communication (IPC) is a cornerstone of a kernel’s operation. In a modern operating system, there are many mechanisms available for processes to communication with one another and synchronize activities. These mechanisms include semaphores, mutexes, pipes, sockets, and more.  

In our THREADS kernel, we begin support for IPC with a mail-based system for communication. This mail-based system enables processes to both send and receive messages and is modelled after a physical mail delivery system where mail (a message) is placed in a mailbox and the mail is  eventually received (“picked up”) by the recipient when it is removed from the mailbox.

The Messaging mailbox system has the following important requirements:

- Mailboxes have a set number of mail slots where each slot can hold one message.
- Mail must be received in the order it is sent.
- Mailboxes may be either blocking or non-blocking.

### Blocking Mailboxes

- A sending process must block when sending a message until a mail slot is available to hold the message in the mailbox.
- A receiving process must block if there is no mail in the mailbox.

### Non-blocking Mailboxes

- Both the sender and the receiver return immediately regardless of whether mail was delivered or received. (An error code is used to indicate successful delivery or reception.)

### Zero-slot Mailboxes

- Mailboxes can be created with zero slots.

These rules may appear relatively straightforward, but the implementation of such a mailbox system becomes complex in an environment where many processes of different priorities are running at the same time.

## Contributors

| Name | College | Program | Contact |
| --- | --- | --- | --- |
| Anthony Tropeano | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Cyber Operations/Cyber Engineering*](https://azcast.arizona.edu/academics/cyber-operations/cyber-engineering) | [**GitHub**](https://github.com/iiTONELOC) <br> [**Email**](mailto:atropeano@arizona.edu) |
| Connor Stackhouse | [**College of Applied Science and Technology**](https://azcast.arizona.edu/) | [*Cyber Operations/Cyber Engineering*](https://azcast.arizona.edu/academics/cyber-operations/cyber-engineering) | [**GitHub**](https://github.com/con-stackhouse) <br> [**Email**](mailto:cstackhouse@arizona.edu) |

## Usage

Part of the solution utilizes a Generic Linked List implementation, [DoubleSeaLib](https://github.com/iiTONELOC/DoubleSea) created by Anthony, as a DLL, and is excluded from the `threads-kernel` repo.

For simplicity, a ZIP folder has been provided which includes pre-built output and the necessary DLLs and Libs.

The solution folder can be opened in Visual Studio and the project rebuilt if necessary.

Once unzipped, the prebuilt solution and its tests can be executed directly using the Batch file included in the `bin/` folder titled: `RunMessengerTests.bat`:

```bash
cd <path_to_THREADS-Messenger/bin>

.\RunMessengerTests.bat

# expected output

MessagingTest00: started
All processes completed.

MessagingTest01: started
MessagingTest01: mailbox_create returned id = 8
MessagingTest01: mailbox_create returned id = 9
All processes completed.

# ... Truncated Output ...
```

## Solution

### Test Case Results

| Test Case Number | Test Result |
|------------------|-------------|
| 00               |    PASSED   |
| 01               |    PASSED   |
| 02               |    PASSED   |
| 03               |    PASSED*  |
| 04               |    PASSED   |
| 05               |    PASSED   |
| 06               |    PASSED   |
| 07               |    PASSED   |
| 08               |    PASSED   |
| 09               |    PASSED*  |
| 10               |    PASSED   |
| 11               |    PASSED   |
| 12               |    PASSED   |
| 13               |    PASSED   |
| 14               |    PASSED*  |
| 15               |    PASSED   |
| 16               |    PASSED   |
| 17               |    PASSED   |
| 18               |    PASSED   |
| 19               |    PASSED   |
| 20               |    PASSED   |
| 21               |    PASSED   |
| 22               |    PASSED   |
| 23               |    PASSED   |
| 24               |    PASSED   |
| 25               |    PASSED*  |
| 26               |    PASSED   |
| 27               |    PASSED   |
| 28               |    PASSED   |
| 29               |    PASSED   |
| 30               |    PASSED   |
| 31               |    PASSED   |
| 32               |    PASSED   |
| 33               |    PASSED   |
| 34               |    PASSED   |
| 35               |    PASSED   |
| 36               |    PASSED   |
| 37               |    PASSED   |

>Note: Test cases with an * indicated that the test passes but does not match the provided output when piped to a file.

### Test Case Output

```bash

MessagingTest00: started
All processes completed.

MessagingTest01: started
MessagingTest01: mailbox_create returned id = 8
MessagingTest01: mailbox_create returned id = 9
All processes completed.

MessagingTest02: started
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
MessagingTest02: mailbox_create returned id less than zero, id = -1
All processes completed.

MessagingTest03: started
MessagingTest03: mailbox_create returned id = 8
MessagingTest03: sending message to mailbox 8
MessagingTest03: after send of message, result = 0
MessagingTest03: receiving message from mailbox 8
MessagingTest03: after receipt of message, result = 12
MessagingTest03:   message received = hello there
All processes completed.

MessagingTest04: started
MessagingTest04: mailbox_create returned id = 8
MessagingTest04-Child1: started
MessagingTest04-Child1: Delivered message: Message Number 0
MessagingTest04: Exit status for child MessagingTest04-Child1 is -3
MessagingTest04-Child2: started
MessagingTest04-Child2: Received message: Message Number 0
MessagingTest04: Exit status for child MessagingTest04-Child2 is -3
All processes completed.

MessagingTest05: started
MessagingTest05: Returned from waitdevice, status: 0x00000000.
MessagingTest05: mailbox_create returned id = 8
MessagingTest05-Child2: started
MessagingTest05-Child1: started
MessagingTest05-Child2: Received message: Message Number 0
MessagingTest05: Exit status for child MessagingTest05-Child2 is -3
MessagingTest05-Child1: Delivered message: Message Number 0
MessagingTest05: Exit status for child MessagingTest05-Child1 is -3
All processes completed.

MessagingTest06: started
MessagingTest06: mailbox_create returned id = 8
MessagingTest06-Child1: started
MessagingTest06-Child1: Delivered message: Message Number 0
MessagingTest06-Child1: Delivered message: Message Number 1
MessagingTest06-Child1: Delivered message: Message Number 2
MessagingTest06-Child1: Delivered message: Message Number 3
MessagingTest06-Child1: Delivered message: Message Number 4
MessagingTest06-Child2: started
MessagingTest06-Child1: Delivered message: Message Number 5
MessagingTest06: Exit status for child MessagingTest06-Child1 is -3
MessagingTest06-Child2: Received message: Message Number 0
MessagingTest06-Child2: Received message: Message Number 1
MessagingTest06-Child2: Received message: Message Number 2
MessagingTest06-Child2: Received message: Message Number 3
MessagingTest06-Child2: Received message: Message Number 4
MessagingTest06-Child2: Received message: Message Number 5
MessagingTest06: Exit status for child MessagingTest06-Child2 is -3
All processes completed.

MessagingTest07: started
MessagingTest07: mailbox_create returned id = 8
MessagingTest07-Child1: started
MessagingTest07-Child1: Delivered message: Message Number 0
MessagingTest07-Child1: Delivered message: Message Number 1
MessagingTest07-Child1: Delivered message: Message Number 2
MessagingTest07-Child1: Delivered message: Message Number 3
MessagingTest07-Child1: Delivered message: Message Number 4
MessagingTest07: Exit status for child MessagingTest07-Child1 is -3
MessagingTest07-Child2: started
MessagingTest07-Child3: started
MessagingTest07-Child4: started
MessagingTest07-Child5: started
MessagingTest07-Child2: Delivered message: Message Number 5
MessagingTest07: Exit status for child MessagingTest07-Child2 is -3
MessagingTest07-Child5: Received message: Message Number 0
MessagingTest07-Child3: Delivered message: Message Number 6
MessagingTest07: Exit status for child MessagingTest07-Child3 is -3
MessagingTest07-Child5: Received message: Message Number 1
MessagingTest07-Child4: Delivered message: Message Number 7
MessagingTest07: Exit status for child MessagingTest07-Child4 is -3
MessagingTest07-Child5: Received message: Message Number 2
MessagingTest07-Child5: Received message: Message Number 3
MessagingTest07-Child5: Received message: Message Number 4
MessagingTest07-Child5: Received message: Message Number 5
MessagingTest07-Child5: Received message: Message Number 6
MessagingTest07-Child5: Received message: Message Number 7
MessagingTest07: Exit status for child MessagingTest07-Child5 is -3
All processes completed.

MessagingTest08: started
MessagingTest08: mailbox_create returned id = 8
MessagingTest08-Child1: started
MessagingTest08-Child1: Delivered message: Message Number 0
MessagingTest08-Child1: Delivered message: Message Number 1
MessagingTest08-Child1: Delivered message: Message Number 2
MessagingTest08-Child1: Delivered message: Message Number 3
MessagingTest08-Child1: Delivered message: Message Number 4
MessagingTest08: Exit status for child MessagingTest08-Child1 is -3
MessagingTest08-Child2: started
MessagingTest08-Child3: started
MessagingTest08-Child4: started
MessagingTest08-Child5: started
MessagingTest08-Child2: Message delivery failed: result = -5
MessagingTest08: Exit status for child MessagingTest08-Child2 is -5
MessagingTest08-Child3: Message delivery failed: result = -5
MessagingTest08: Exit status for child MessagingTest08-Child3 is -5
MessagingTest08-Child4: Message delivery failed: result = -5
MessagingTest08: Exit status for child MessagingTest08-Child4 is -5
MessagingTest08-Child5: mailbox_free returned 0
MessagingTest08: Exit status for child MessagingTest08-Child5 is -3
All processes completed.

MessagingTest09: started
MessagingTest09: mailbox_create returned id = 8
MessagingTest09-Child1: started
MessagingTest09-Child1: Delivered message: Message Number 0
MessagingTest09-Child1: Delivered message: Message Number 1
MessagingTest09-Child1: Delivered message: Message Number 2
MessagingTest09-Child1: Delivered message: Message Number 3
MessagingTest09-Child1: Delivered message: Message Number 4
MessagingTest09: Exit status for child MessagingTest09-Child1 is -3
MessagingTest09-Child2: started
MessagingTest09-Child3: started
MessagingTest09-Child4: started
MessagingTest09-Child5: started
MessagingTest09-Child6: started
MessagingTest09-Child2: Message delivery failed: result = -5
MessagingTest09: Exit status for child MessagingTest09-Child2 is -5
MessagingTest09-Child3: Message delivery failed: result = -5
MessagingTest09: Exit status for child MessagingTest09-Child3 is -5
MessagingTest09-Child4: Message delivery failed: result = -5
MessagingTest09: Exit status for child MessagingTest09-Child4 is -5
MessagingTest09-Child6: mailbox_free returned 0
MessagingTest09: Exit status for child MessagingTest09-Child6 is -3
MessagingTest09: Exit status for child @╗o√R is -3
All processes completed.

MessagingTest10: started
MessagingTest10: mailbox_create returned id = 8
MessagingTest10-Child1: started
MessagingTest10-Child1: Delivered message: Message Number 0
MessagingTest10-Child1: Delivered message: Message Number 1
MessagingTest10-Child1: Delivered message: Message Number 2
MessagingTest10-Child1: Delivered message: Message Number 3
MessagingTest10-Child1: Delivered message: Message Number 4
MessagingTest10: Exit status for child MessagingTest10-Child1 is -3
MessagingTest10-Child2: started
MessagingTest10-Child2: Message delivery failed: result = -2
MessagingTest10: Exit status for child MessagingTest10-Child2 is -3
MessagingTest10-Child3: started
MessagingTest10-Child4: started
MessagingTest10-Child5: started
MessagingTest10-Child3: Delivered message: Message Number 6
MessagingTest10: Exit status for child MessagingTest10-Child3 is -3
MessagingTest10-Child5: Received message: Message Number 0
MessagingTest10-Child4: Delivered message: Message Number 7
MessagingTest10: Exit status for child MessagingTest10-Child4 is -3
MessagingTest10-Child5: Received message: Message Number 1
MessagingTest10-Child5: Received message: Message Number 2
MessagingTest10-Child5: Received message: Message Number 3
MessagingTest10-Child5: Received message: Message Number 4
MessagingTest10-Child5: Received message: Message Number 6
MessagingTest10: Exit status for child MessagingTest10-Child5 is -3
All processes completed.

MessagingTest11: started
MessagingTest11: mailbox_create returned id = 8
MessagingTest11: mailbox_create returned id = 9
MessagingTest11-Child1: started
MessagingTest11-Child1: Sending message 0 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 0' DELIVERED
MessagingTest11-Child1: Sending message 1 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 1' DELIVERED
MessagingTest11-Child1: Sending message 2 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 2' DELIVERED
MessagingTest11-Child1: Sending message 3 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 3' DELIVERED
MessagingTest11-Child1: Sending message 4 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 4' DELIVERED
MessagingTest11-Child1: Sending message 5 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned -2
MessagingTest11-Child1: Sending message 6 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned -2
MessagingTest11-Child1: Sending message 7 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned -2
MessagingTest11-Child2: started
MessagingTest11-Child2: Receiving message 0 from mailbox 8
MessagingTest11-Child2: Received message 0, result = 14, message = Hello There 0
MessagingTest11-Child2: Receiving message 1 from mailbox 8
MessagingTest11-Child2: Received message 1, result = 14, message = Hello There 1
MessagingTest11-Child2: Receiving message 2 from mailbox 8
MessagingTest11-Child2: Received message 2, result = 14, message = Hello There 2
MessagingTest11-Child2: Receiving message 3 from mailbox 8
MessagingTest11-Child2: Received message 3, result = 14, message = Hello There 3
MessagingTest11-Child2: Receiving message 4 from mailbox 8
MessagingTest11-Child2: Received message 4, result = 14, message = Hello There 4
MessagingTest11-Child1: Sending message 8 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 8' DELIVERED
MessagingTest11-Child1: Sending message 9 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 9' DELIVERED
MessagingTest11-Child1: Sending message 10 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 10' DELIVERED
MessagingTest11-Child1: Sending message 11 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 11' DELIVERED
MessagingTest11-Child1: Sending message 12 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned 0 - Message 'Hello There 12' DELIVERED
MessagingTest11-Child1: Sending message 13 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned -2
MessagingTest11-Child1: Sending message 14 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned -2
MessagingTest11-Child1: Sending message 15 to mailbox 8
MessagingTest11-Child1: mailbox_send Returned -2
MessagingTest11: Exit status for child MessagingTest11-Child1 is -3
MessagingTest11-Child2: Receiving message 0 from mailbox 8
MessagingTest11-Child2: Received message 0, result = 14, message = Hello There 8
MessagingTest11-Child2: Receiving message 1 from mailbox 8
MessagingTest11-Child2: Received message 1, result = 14, message = Hello There 9
MessagingTest11-Child2: Receiving message 2 from mailbox 8
MessagingTest11-Child2: Received message 2, result = 15, message = Hello There 10
MessagingTest11-Child2: Receiving message 3 from mailbox 8
MessagingTest11-Child2: Received message 3, result = 15, message = Hello There 11
MessagingTest11-Child2: Receiving message 4 from mailbox 8
MessagingTest11-Child2: Received message 4, result = 15, message = Hello There 12
MessagingTest11: Exit status for child MessagingTest11-Child2 is -3
All processes completed.

MessagingTest12: started
MessagingTest12: mailbox_create returned id = 8
MessagingTest12: mailbox_create returned id = 9
MessagingTest12-Child1: started
MessagingTest12-Child1: Sending message 0 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 0' DELIVERED
MessagingTest12-Child1: Sending message 1 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 1' DELIVERED
MessagingTest12-Child1: Sending message 2 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 2' DELIVERED
MessagingTest12-Child1: Sending message 3 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 3' DELIVERED
MessagingTest12-Child1: Sending message 4 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 4' DELIVERED
MessagingTest12-Child1: Sending message 5 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned -2
MessagingTest12-Child1: Sending message 6 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned -2
MessagingTest12-Child1: Sending message 7 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned -2
MessagingTest12-Child2: started
MessagingTest12-Child2: Received message in non-blocking mode, result = 14, message = Hello There 0
MessagingTest12-Child2: Received message in non-blocking mode, result = 14, message = Hello There 1
MessagingTest12-Child2: Received message in non-blocking mode, result = 14, message = Hello There 2
MessagingTest12-Child2: Received message in non-blocking mode, result = 14, message = Hello There 3
MessagingTest12-Child2: Received message in non-blocking mode, result = 14, message = Hello There 4
MessagingTest12-Child2: Received 5 hello messages from mailbox 8
MessagingTest12-Child1: Sending message 8 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 8' DELIVERED
MessagingTest12-Child1: Sending message 9 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 9' DELIVERED
MessagingTest12-Child1: Sending message 10 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 10' DELIVERED
MessagingTest12-Child1: Sending message 11 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 11' DELIVERED
MessagingTest12-Child1: Sending message 12 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned 0 - Message 'Hello There 12' DELIVERED
MessagingTest12-Child1: Sending message 13 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned -2
MessagingTest12-Child1: Sending message 14 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned -2
MessagingTest12-Child1: Sending message 15 to mailbox 8
MessagingTest12-Child1: mailbox_send Returned -2
MessagingTest12: Exit status for child MessagingTest12-Child1 is -3
MessagingTest12-Child2: Received message in non-blocking mode, result = 14, message = Hello There 8
MessagingTest12-Child2: Received message in non-blocking mode, result = 14, message = Hello There 9
MessagingTest12-Child2: Received message in non-blocking mode, result = 15, message = Hello There 10
MessagingTest12-Child2: Received message in non-blocking mode, result = 15, message = Hello There 11
MessagingTest12-Child2: Received message in non-blocking mode, result = 15, message = Hello There 12
MessagingTest12-Child2: Received 5 hello messages from mailbox 8
MessagingTest12: Exit status for child MessagingTest12-Child2 is -3
All processes completed.

MessagingTest13: started
MessagingTest13-Child1: started
MessagingTest13-Child1: Returned from waitdevice, status: 0x00000000.
MessagingTest13: Exit status for child MessagingTest13-Child1 is -3
All processes completed.

MessagingTest14: started
MessagingTest14-Child1: started
MessagingTest14-Child1: Returned from waitdevice, result: 0, status: 0x00000100.
MessagingTest14: exit status for child 4 is -3
All processes completed.

MessagingTest15: started
MessagingTest15: mailbox_create returned id = 8
MessagingTest15: Sending large message to mailbox 8
MessagingTest15: mailbox_send failed
All processes completed.

MessagingTest16: started
All processes completed.

MessagingTest17: started
MessagingTest17: mailbox_create returned id = 8
MessagingTest17-Child1: started
MessagingTest17-Child1: Sending message to mailbox 8
MessagingTest17-Child2: started
MessagingTest17-Child2: Sending message to mailbox 8
MessagingTest17-Child3: started
MessagingTest17-Child3: Sending message to mailbox 8
MessagingTest17-Child4: started
MessagingTest17-Child4: quitting
MessagingTest17: Exit status for MessagingTest17-Child4 is -3
MessagingTest17-Child5: started
MessagingTest17: Exit status for MessagingTest17-Child1 is -5
MessagingTest17: Exit status for MessagingTest17-Child2 is -5
MessagingTest17: Exit status for MessagingTest17-Child3 is -5
MessagingTest17-Child5: mailbox_close returned 0
MessagingTest17: Exit status for MessagingTest17-Child5 is -3
All processes completed.

MessagingTest18: started
MessagingTest18: mailbox_create returned id = 8
MessagingTest18-Child1: started
MessagingTest18-Child1: Receiving message from mailbox 8
MessagingTest18-Child2: started
MessagingTest18-Child2: Receiving message from mailbox 8
MessagingTest18-Child3: started
MessagingTest18-Child3: Receiving message from mailbox 8
MessagingTest18-Child4: started
MessagingTest18-Child4: quitting
MessagingTest18: Exit status for MessagingTest18-Child4 is -3
MessagingTest18-Child5: started
MessagingTest18: Exit status for MessagingTest18-Child1 is -5
MessagingTest18: Exit status for MessagingTest18-Child2 is -5
MessagingTest18: Exit status for MessagingTest18-Child3 is -5
MessagingTest18-Child5: mailbox_close returned 0
MessagingTest18: Exit status for MessagingTest18-Child5 is -3
All processes completed.

MessagingTest19: started
MessagingTest19: mailbox_create returned id = 8
MessagingTest19-Child1: started
MessagingTest19-Child1: Sending message to mailbox 8
MessagingTest19-Child2: started
MessagingTest19-Child2: Sending message to mailbox 8
MessagingTest19-Child3: started
MessagingTest19-Child3: Sending message to mailbox 8
MessagingTest19-Child4: started
MessagingTest19-Child4: quitting
MessagingTest19: Exit status for MessagingTest19-Child4 is -3
MessagingTest19-Child5: started
MessagingTest19: Exit status for MessagingTest19-Child1 is -5
MessagingTest19: Exit status for MessagingTest19-Child2 is -5
MessagingTest19: Exit status for MessagingTest19-Child3 is -5
MessagingTest19-Child5: mailbox_close returned 0
MessagingTest19: Exit status for MessagingTest19-Child5 is -3
MessagingTest19: Message send failed on released mailbox - SUCCESS
All processes completed.

MessagingTest20: started
MessagingTest20: mailbox_create returned id = 8
MessagingTest20-Child1: started
MessagingTest20-Child1: Receiving message from mailbox 8
MessagingTest20-Child2: started
MessagingTest20-Child2: Receiving message from mailbox 8
MessagingTest20-Child3: started
MessagingTest20-Child3: Receiving message from mailbox 8
MessagingTest20-Child4: started
MessagingTest20-Child4: quitting
MessagingTest20: Exit status for MessagingTest20-Child4 is -3
MessagingTest20-Child5: started
MessagingTest20: Exit status for MessagingTest20-Child1 is -5
MessagingTest20: Exit status for MessagingTest20-Child2 is -5
MessagingTest20: Exit status for MessagingTest20-Child3 is -5
MessagingTest20-Child5: mailbox_close returned 0
MessagingTest20: Exit status for MessagingTest20-Child5 is -3
MessagingTest20: Message receive failed on released mailbox - SUCCESS
All processes completed.

MessagingTest21: started
MessagingTest21: mailbox_create returned id = 8
MessagingTest21-Child1: started
MessagingTest21-Child1: Sending message 0 to mailbox 8
MessagingTest21-Child2: started
MessagingTest21-Child2: Receiving message 0 from mailbox 8
MessagingTest21-Child1: Delivered message - result = 0, message = Hello There, 0
MessagingTest21-Child1: Sending message 1 to mailbox 8
MessagingTest21-Child2: Received message 0, result = 15, message = Hello There, 0
MessagingTest21-Child2: Receiving message 1 from mailbox 8
MessagingTest21-Child1: Delivered message - result = 0, message = Hello There, 1
MessagingTest21: Exit status for child MessagingTest21-Child1 is -3
MessagingTest21-Child2: Received message 1, result = 15, message = Hello There, 1
MessagingTest21: Exit status for child MessagingTest21-Child2 is -3
All processes completed.

MessagingTest22: started
MessagingTest22: mailbox_create returned id = 8
MessagingTest22-Child2: started
MessagingTest22-Child2: Receiving message 0 from mailbox 8
MessagingTest22-Child1: started
MessagingTest22-Child1: Sending message 0 to mailbox 8
MessagingTest22-Child2: Received message 0, result = 15, message = Hello There, 0
MessagingTest22-Child2: Receiving message 1 from mailbox 8
MessagingTest22-Child1: Delivered message - result = 0, message = Hello There, 0
MessagingTest22-Child1: Sending message 1 to mailbox 8
MessagingTest22-Child2: Received message 1, result = 15, message = Hello There, 1
MessagingTest22: Exit status for child MessagingTest22-Child2 is -3
MessagingTest22-Child1: Delivered message - result = 0, message = Hello There, 1
MessagingTest22: Exit status for child MessagingTest22-Child1 is -3
All processes completed.

MessagingTest23: started
MessagingTest23: mailbox_create returned id = 8
MessagingTest23-Child1: Spawning ReceiveAndExit at priority 2
MessagingTest23-Child1-Child1: started
MessagingTest23-Child1-Child1: receiving message from mailbox 8
MessagingTest23-Child1: Spawning ReceiveAndExit at priority 4
MessagingTest23-Child1-Child2: started
MessagingTest23-Child1-Child2: receiving message from mailbox 8
MessagingTest23-Child1: Spawning ReceiveAndExit at priority 4
MessagingTest23-Child1-Child3: started
MessagingTest23-Child1-Child3: receiving message from mailbox 8
MessagingTest23-Child1: Spawning SendThreeAndExit at priority 5
MessagingTest23-Child1-Child4: started
MessagingTest23-Child1-Child4: Sending message 0 to mailbox 8
MessagingTest23-Child1-Child4: Delivered message - result = 0, message = Message Number 0
MessagingTest23-Child1-Child4: Sending message 1 to mailbox 8
MessagingTest23-Child1-Child4: Delivered message - result = 0, message = Message Number 1
MessagingTest23-Child1-Child4: Sending message 2 to mailbox 8
MessagingTest23-Child1-Child4: Delivered message - result = 0, message = Message Number 2
MessagingTest23-Child1-Child2: message received: Message Number 1
MessagingTest23-Child1-Child3: message received: Message Number 2
MessagingTest23-Child1-Child1: message received: Message Number 0
MessagingTest23-Child1: Exit status for child MessagingTest23-Child1-Child4 is -3
MessagingTest23-Child1: Exit status for child MessagingTest23-Child1-Child2 is -3
MessagingTest23-Child1: Exit status for child MessagingTest23-Child1-Child3 is -3
MessagingTest23-Child1: Exit status for child MessagingTest23-Child1-Child1 is -3
MessagingTest23: Exit status for child MessagingTest23-Child1 is -3
All processes completed.

MessagingTest24: started
MessagingTest24: mailbox_create returned id = 8
MessagingTest24-Child1: started
MessagingTest24-Child1: Delivered message: Message Number 0
MessagingTest24-Child1: Delivered message: Message Number 1
MessagingTest24-Child1: Delivered message: Message Number 2
MessagingTest24-Child1: Delivered message: Message Number 3
MessagingTest24-Child1: Delivered message: Message Number 4
MessagingTest24-Child1: Spawning SendOneAndExit at priority 2
MessagingTest24-Child1-Child1: started
MessagingTest24-Child1: Spawning SendOneAndExit at priority 3
MessagingTest24-Child1-Child2: started
MessagingTest24-Child1: Spawning SendOneAndExit at priority 4
MessagingTest24-Child1-Child3: started
MessagingTest24-Child1: Spawning ReceiveEightAndExit at priority 5
MessagingTest24-Child1-Child4: started
MessagingTest24-Child1-Child4: message received: Message Number 0
MessagingTest24-Child1-Child4: message received: Message Number 1
MessagingTest24-Child1-Child4: message received: Message Number 2
MessagingTest24-Child1-Child4: message received: Message Number 3
MessagingTest24-Child1-Child4: message received: Message Number 4
MessagingTest24-Child1-Child4: message received: Message Number 5
MessagingTest24-Child1-Child4: message received: Message Number 6
MessagingTest24-Child1-Child4: message received: Message Number 7
MessagingTest24-Child1-Child3: Delivered message: Message Number 7
MessagingTest24-Child1-Child2: Delivered message: Message Number 6
MessagingTest24-Child1-Child1: Delivered message: Message Number 5
MessagingTest24-Child1: Exit status for child MessagingTest24-Child1-Child4 is -3
MessagingTest24-Child1: Exit status for child MessagingTest24-Child1-Child3 is -3
MessagingTest24-Child1: Exit status for child MessagingTest24-Child1-Child2 is -3
MessagingTest24-Child1: Exit status for child MessagingTest24-Child1-Child1 is -3
MessagingTest24: Exit status for child MessagingTest24-Child1 is -3
All processes completed.

MessagingTest25: started

MessagingTest25: trying to create too many mailboxes
MessagingTest25: mailbox_create returned id less than zero, id = -1
MessagingTest25: mailbox_create returned id less than zero, id = -1
MessagingTest25: mailbox_create returned id less than zero, id = -1
MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_close returned 0

MessagingTest25: mailbox_create returned id = 2007
MessagingTest25: mailbox_create returned id = 2008
MessagingTest25: mailbox_create returned id = 2009
MessagingTest25: mailbox_create returned id = 2010
MessagingTest25: mailbox_create returned id = 2011
MessagingTest25: mailbox_create returned id = 2012
MessagingTest25: mailbox_create returned id = 2013
MessagingTest25: mailbox_create returned id = 2014
MessagingTest25: mailbox_create returned id = 2015
MessagingTest25: mailbox_create returned id = 2016
MessagingTest25: mailbox_create returned id less than zero, id = -1
MessagingTest25: mailbox_create returned id less than zero, id = -1
MessagingTest25: mailbox_create returned id less than zero, id = -1
MessagingTest25: mailbox_create returned id less than zero, id = -1
All processes completed.

MessagingTest26: started
MessagingTest26: mailbox_create returned id = 8
MessagingTest26: Spawned MessagingTest26-Child1 with priority 4
MessagingTest26: Spawned MessagingTest26-Child2 with priority 3
MessagingTest26: Spawned MessagingTest26-Child3 with priority 2
MessagingTest26: Spawned MessagingTest26-Child4 with priority 3
MessagingTest26: Spawned MessagingTest26-Child5 with priority 1
MessagingTest26-Child1: started
MessagingTest26-Child2: started
MessagingTest26-Child4: started
MessagingTest26-Child3: started
MessagingTest26-Child5: started
MessagingTest26-Child1: Message receive failed: result = -5
MessagingTest26-Child1: Message receive failed: result = -1
MessagingTest26-Child1: Message receive failed: result = -1
MessagingTest26-Child1: Message receive failed: result = -1
MessagingTest26-Child1: Message receive failed: result = -1
MessagingTest26-Child1: Message delivery failed: result = -1
MessagingTest26-Child1: Message delivery failed: result = -1
MessagingTest26-Child1: Message delivery failed: result = -1
MessagingTest26-Child1: Message delivery failed: result = -1
MessagingTest26-Child1: Message delivery failed: result = -1
MessagingTest26: Exit status for child MessagingTest26-Child1 is -5
MessagingTest26-Child2: Message receive failed: result = -5
MessagingTest26: Exit status for child MessagingTest26-Child2 is -5
MessagingTest26-Child4: Message receive failed: result = -5
MessagingTest26: Exit status for child MessagingTest26-Child4 is -5
MessagingTest26-Child3: Message receive failed: result = -5
MessagingTest26: Exit status for child MessagingTest26-Child3 is -5
MessagingTest26-Child5: mailbox_free returned 0
MessagingTest26: Exit status for child MessagingTest26-Child5 is -3
All processes completed.

MessagingTest27: started
MessagingTest27: mailbox_create returned id = 8
MessagingTest27: Spawned MessagingTest27-Child1 with priority 4
MessagingTest27: Spawned MessagingTest27-Child2 with priority 3
MessagingTest27: Spawned MessagingTest27-Child3 with priority 2
MessagingTest27: Spawned MessagingTest27-Child4 with priority 3
MessagingTest27: Spawned MessagingTest27-Child5 with priority 3
MessagingTest27-Child4: started
MessagingTest27-Child3: started
MessagingTest27-Child2: started
MessagingTest27-Child1: started
MessagingTest27-Child5: started
MessagingTest27: Exit status for child MessagingTest27-Child5 is -3
MessagingTest27: Spawned MessagingTest27-Child6 with priority 1
MessagingTest27-Child6: started
MessagingTest27-Child4: Message receive failed: result = -5
MessagingTest27: Exit status for child MessagingTest27-Child4 is -5
MessagingTest27-Child3: Message receive failed: result = -5
MessagingTest27: Exit status for child MessagingTest27-Child3 is -5
MessagingTest27-Child2: Message receive failed: result = -5
MessagingTest27: Exit status for child MessagingTest27-Child2 is -5
MessagingTest27-Child1: Message receive failed: result = -5
MessagingTest27-Child1: Message receive failed: result = -1
MessagingTest27-Child1: Message delivery failed: result = -1
MessagingTest27-Child1: Message delivery failed: result = -1
MessagingTest27: Exit status for child MessagingTest27-Child1 is -5
MessagingTest27-Child6: mailbox_free returned 0
MessagingTest27: Exit status for child MessagingTest27-Child6 is -3
All processes completed.

MessagingTest28: started
MessagingTest28: mailbox_create returned id = 8
MessagingTest28-Child1: started
MessagingTest28-Child2: started
MessagingTest28-Child3: started
MessagingTest28-Child4: started
MessagingTest28-Child1: Message receive failed: result = -5
MessagingTest28: Exit status for child MessagingTest28-Child1 is -5
MessagingTest28-Child2: Message receive failed: result = -5
MessagingTest28: Exit status for child MessagingTest28-Child2 is -5
MessagingTest28-Child3: Message receive failed: result = -5
MessagingTest28: Exit status for child MessagingTest28-Child3 is -5
MessagingTest28-Child4: mailbox_free returned 0
MessagingTest28: Exit status for child MessagingTest28-Child4 is -3
MessagingTest28: mailbox_receive returned -1: SUCCESS
All processes completed.

MessagingTest29: started
MessagingTest29: mailbox_create returned id = 8
MessagingTest29-Child1: started
MessagingTest29-Child2: started
MessagingTest29-Child3: started
MessagingTest29-Child4: started
MessagingTest29-Child1: Message delivery failed: result = -5
MessagingTest29: Exit status for child MessagingTest29-Child1 is -5
MessagingTest29-Child2: Message delivery failed: result = -5
MessagingTest29: Exit status for child MessagingTest29-Child2 is -5
MessagingTest29-Child3: Message delivery failed: result = -5
MessagingTest29: Exit status for child MessagingTest29-Child3 is -5
MessagingTest29-Child4: mailbox_free returned 0
MessagingTest29: Exit status for child MessagingTest29-Child4 is -3
MessagingTest29: mailbox_send returned -1: SUCCESS
All processes completed.

MessagingTest30: started
MessagingTest30: mailbox_create returned id = 8
MessagingTest30: Delivered message: First message
MessagingTest30-Child3: started
MessagingTest30-Child3: Message delivery failed: result = -1
MessagingTest30: Exit status for child MessagingTest30-Child3 is -3
MessagingTest30-Child2: started
MessagingTest30-Child2: Message delivery failed: result = -1
MessagingTest30: Exit status for child MessagingTest30-Child2 is -3
MessagingTest30-Child1: started
MessagingTest30-Child1: Message delivery failed: result = -1
MessagingTest30: Exit status for child MessagingTest30-Child1 is -3
MessagingTest30-Child4: started
MessagingTest30-Child4: mailbox_free returned 0
MessagingTest30: Exit status for child MessagingTest30-Child4 is -3
All processes completed.

MessagingTest31: started
MessagingTest31: mailbox_create returned id = 8
MessagingTest31-Child3: started
MessagingTest31-Child2: started
MessagingTest31-Child1: started
MessagingTest31-Child4: started
MessagingTest31: Message receive failed: result = -5
MessagingTest31-Child3: Message receive failed: result = -5
MessagingTest31: Exit status for child  is -5
MessagingTest31-Child2: Message receive failed: result = -5
MessagingTest31: Exit status for child  is -5
MessagingTest31-Child1: Message receive failed: result = -5
MessagingTest31: Exit status for child  is -5
MessagingTest31-Child4: mailbox_free returned 0
MessagingTest31: Exit status for child  is -3
All processes completed.

MessagingTest32: started
MessagingTest32-Child1: started
MessagingTest32-Child1: started
MessagingTest32-Child1: started
MessagingTest32-Child1: started
MessagingTest32-Child1: wait_device returned 0 with status 0x00000000
MessagingTest32: Exit status for child MessagingTest32-Child1 is -3
MessagingTest32-Child1: wait_device returned 0 with status 0x00000000
MessagingTest32: Exit status for child MessagingTest32-Child1 is -3
MessagingTest32-Child1: wait_device returned 0 with status 0x00000000
MessagingTest32: Exit status for child MessagingTest32-Child1 is -3
MessagingTest32-Child1: wait_device returned 0 with status 0x00000000
MessagingTest32: Exit status for child MessagingTest32-Child1 is -3
All processes completed.

MessagingTest33: started
MessagingTest33: mailbox_create returned id = 8

MessagingTest33: Sending message of max mailbox size
MessagingTest33: Delivered message: Test Message
MessagingTest33: Received message: Test Message
All processes completed.

MessagingTest34: started
MessagingTest34: mailbox_create returned id = 8

MessagingTest34: Sending message of max mailbox size + 1
MessagingTest34: Message delivery failed: result = -1
All processes completed.

MessagingTest35: started
MessagingTest35: mailbox_create returned id = -1
MessagingTest35: mailbox_create border case test 1: PASSED
MessagingTest35: mailbox_create returned id = -1
MessagingTest35: mailbox_create border case test 2: PASSED
MessagingTest35: mailbox_create returned id = -1
MessagingTest35: mailbox_create border case test 3: PASSED
All processes completed.

MessagingTest36: started
MessagingTest36: mailbox_create returned id = 8

MessagingTest36: Receiving message with a buffer that is too small
MessagingTest36-Child1: started
MessagingTest36: Message receive failed: result = -1
MessagingTest36-Child1: Delivered message: Message Number 0
All processes completed.

MessagingTest37: started
MessagingTest37: mailbox_create returned id = 8
MessagingTest37: Delivered message: Test Message

MessagingTest37: Receiving message with a buffer that is too small
MessagingTest37: Message receive failed: result = -1
All processes completed.

```
