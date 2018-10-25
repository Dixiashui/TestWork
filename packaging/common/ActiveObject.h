#ifndef ACTIVEOBJECT_H_
#define ACTIVEOBJECT_H_

#include <list>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <boost/shared_ptr.hpp>
#include "common/Mutex.h"
#include "SafeQueue.h"
#include "TwException.h"
#include "SyncReturnObject.h"

#define ACTIVE_OBJECT_POLLING_TIME 250000
#define ACTIVE_OBJECT_SLEEP_TIME   500000

// defines the ActiveObject's methods return type for asynchonous calls
template<typename T>
class  ActiveObjectReturn : public SyncReturnObject<T>
{
};

/**
 *  @brief Implements an object that will work as an active object. 
 *  
 * An active object is one that "runs in thread" or in multiple threads and
 * that can be accessed by multiple threads safely. The active object's
 * methods that should be ran in separate threads return immediately, but
 * its actual processing will occur in a handling thread. Because of this,
 * the method's return is actually a ActiveObjectReturn that can be used to
 * synchronize the caller when appropriate (and if needed).
 *
 * The ActiveObject class should never have an object instantiated. Instead,
 * one should specialize the class and implement its methods.
 *
 * The methods that will run asynchronously should be implemented like this:
 	\code
		ActiveObjectReturn<return_type> methodName(...) {
			return this->QueueCommand(SOME_ACTION_CODE, parameters);
		}
	\endcode
 *
 * The specialized class shall also implement the HandleCommands method,
 * which will actually run in a separate thread and will be called when the
 * thread becomes idle.
 */
class ActiveObject
{
public:	
// subclasses and structs
	// specifies the structure that holds commands
	/**
	 * @brief A structure used to store commands in the command queue.
	 */
	struct CommandType
	{
		int op_code;///< A code used to distiguish the command.
		void* args;///< A valist with the command's arguments, passed on QueueCommand
		boost::shared_ptr<void> command_return;///< A pointer to a ActiveObjectReturn object
	};	
	
	struct MessageBlock 
	{
		int messageType;
		void * messageData;
		int messageSize;
	};

	// Auxiliar class to encapsulate pointers to commands.
	class CommandPtr {
	public:
		CommandType *ptr;

	public:
		CommandPtr(CommandType *obj = NULL){
			ptr = obj;
		}
				
	};

	/// The command queue's type.
	typedef SafeQueue<CommandPtr> CommandTypeQueue;
	/**
	 * @todo Implement a message queue for thread communication. 
	 */
	typedef SafeQueue<MessageBlock *> MessageQueue;


public:
// ActiveObject's methods implementation
	ActiveObject();
	virtual ~ActiveObject();
	int run();
	int stop();
	inline bool isRunning() {
		return running.Get();
	}
protected:
	/**
	 *  @brief Queues a command in the processing queue.
	 *
	 * The QueueCommand method should be invoked by the specialized class
	 * methods that will execute the method in a separate thread. 
	 * @param _ReturnType The type of return for the given command
	 * @param op_code A code associated to the command. It is the
	 * 	specialized class's responsibility to define the code meaning.
	 * @param args A pointer to the parameters. 
	 * 
	 * @see CommandType
	 * @see HandleCommand
	 */
	template <typename _ReturnType>
		boost::shared_ptr<ActiveObjectReturn<_ReturnType> > QueueCommand(int op_code, void * args = NULL)
		{
			CommandType *command_to_insert;

			// creates a new command object to hold the 
			// requested command in the queue
			command_to_insert = new CommandType();
			command_to_insert->op_code = op_code;

			boost::shared_ptr<ActiveObjectReturn<_ReturnType> > rt =
				boost::shared_ptr<ActiveObjectReturn<_ReturnType> >(
					new ActiveObjectReturn<_ReturnType>()
					);
			command_to_insert->command_return = rt;
			command_to_insert->args = args;

			// insert the command in the queue
			CommandPtr p;
			p.ptr = command_to_insert;
			this->commandQueue.queue(p);

			// command queued successfully 	
			return rt;

		}

	/**
	 * @brief Method invoked when there's an idle thread capable of
	 * processing a command.
	 *
	 * Implement this method in the specialized class to handle the queued
	 * commands.
	 *
	 * @returns Whether the command was succesful.
	 * @see ProcessCommands
	 * 
	 */
	virtual int HandleCommand(CommandType *)=0;

	int pendingCommands() {
		return commandQueue.size();
	}
	
private:
	pthread_t thread_id;///< The thread id, as set by pthread_create
	ActiveObjectSafeAttribute<int> running;///< Whether the thread is running

	// the command queue
	CommandTypeQueue commandQueue; ///< The command queue
	CommandType * DequeueCommand();///< Returns a queued command.
	
	// This is the static method that will be executed in a thread to process
	// the commands sent to this active object
	static void * ProcessCommandsHook(void *void_myself);

	void ProcessCommands();
};



#endif /*ACTIVEOBJECT_H_*/
