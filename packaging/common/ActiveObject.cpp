#include "ActiveObject.h"

/**
 *  @brief The ActiveObject's constructor
 */
ActiveObject::ActiveObject() {
    this->running = 0;
    this->thread_id = 0;
}

/**
 * @brief The ActiveObject's destructor
 *
 * The destructor will check whether the thread is running. If it is, it
 * will stop the thread before destroying the object.
 */
ActiveObject::~ActiveObject() {
    // make sure the thread has ended its task
    if (this->running.Get() && this->thread_id) {
        this->stop();
    }
}

/**
 * @brief Starts the object's thread
 *
 * @returns Whether the thread started succesfully.
 */
int ActiveObject::run() {
    this->running = 1;

    // creates the thread that will process the commands sent to this object
    return pthread_create(&this->thread_id, NULL, ActiveObject::ProcessCommandsHook, this);
}

/**
 * @brief Retrives the next command in the queue.
 *
 * @returns Returns a pointer to a command (of type
 * ActiveObject::CommandType).
 */
ActiveObject::CommandType * ActiveObject::DequeueCommand() {
    CommandPtr p = this->commandQueue.dequeue();
    return p.ptr;
}

/**
 * @brief A static method just used as a callback by the pthread_create
 * function.
 *
 * @returns always NULL.
 */
void * ActiveObject::ProcessCommandsHook(void *void_myself) {
    // just cast it to myself and process the commands; 
    ((ActiveObject*) void_myself)->ProcessCommands();
    return NULL;
}

/**
 *  @brief Process incoming commands.
 *
 * This method will check for commands in the commandQueue. If no commands
 * are avaialable, the object's thread will sleep. Otherwise, the command
 * will be dequeued and will be processed by the HandleCommand method.
 *
 */
void ActiveObject::ProcessCommands() {
    // process commands until the end of times
    while (this->running.Get()) {
        // the command lock is just used to make this thread 'sleep' 
        this->commandQueue.waitForData();

        if (!this->running.Get()) {
            // this lock was released due to the stop command, let's go away
            break;
        }
        // retrieve the command in the queue
        CommandType * new_command = this->DequeueCommand();
        if (new_command) {
            if (this->HandleCommand(new_command) < 0) {
                this->running = 0;
            }
            delete new_command;
        }
    }
    this->running = 0;
}

/**
 * @brief Stops the object's thread, if that is running. 
 */
int ActiveObject::stop() {
    if (this->running.Get()) {
        // stop the execution
        this->running = 0;

        // 'wake' up the process command's thread, forcing the command queue
        // unlock regardless whether there's data or not
        this->commandQueue.ForceUnlock();

        pthread_join(this->thread_id, NULL);
    }

    this->thread_id = 0;
    return 0;
}
