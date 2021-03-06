#ifndef __LOKI_LOKINET_PROCESS_MANAGER_HPP__
#define __LOKI_LOKINET_PROCESS_MANAGER_HPP__

#include <QObject>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

/**
 * An abstract class for dealing with the lokinet process.
 *
 * This class provides a public interface for starting and stopping the lokinet
 * process and delegates to platform-specific subclasses for implementation of
 * actual process management.
 */
class LokinetProcessManager : public QObject
{
    Q_OBJECT

public:

    enum class ProcessStatus
    {
        Unknown = 0,
        Starting,
        Running,
        Stopping,
        Stopped,
    };

    /**
     * Constructor
     */
    LokinetProcessManager();
    
    /**
     * Start the lokinet process.
     *
     * @return false if an error occurs or the process is already running,
     *         true otherwise
     */
    bool startLokinetProcess();
    
    /**
     * Stop the lokinet process.
     *
     * @return false if an error occurs or the process is not running,
     *         true otherwise
     */
    bool stopLokinetProcess();
    
    /**
     * Forcibly stop the lokinet process. Should only be called if a normal
     * stop is insufficient.
     *
     * @return false if an error occurs or the process is not running,
     *         true otherwise
     */
    bool forciblyStopLokinetProcess();

    /**
     * Stop the process in a managed way. The process will immediately be stopped
     * and will be given a short amount of time to gracefully exit, after which
     * it will be forcefully terminated.
     *
     * This will spawn a thread which will manage the termination process. Only
     * one thread may exist at a time; the function will return false if there
     * is already an outstanding thread.
     *
     * @return false if an error occurs or the process is not running or there
     *         is already managed stop process, true otherwise
     */
    bool managedStopLokinetProcess();

    /**
     * Query the status of the process. This should query the OS for a realtime
     * status.
     *
     * @return an up-to-date ProcessStatus, possibly ProcessStatus::Unknown on
     *         error
     */
    ProcessStatus queryProcessStatus();

    /**
     * Returns an appropriate platform-specific instance of this class.
     */
    static LokinetProcessManager* instance();

protected:
    
    /**
     * Subclasses should provide platform-specific means of starting the
     * lokinet process.
     *
     * @return true on success; false otherwise
     */
    virtual bool doStartLokinetProcess() = 0;
    
    /**
     * Subclasses should provide platform-specific means of stopping the
     * lokinet process.
     *
     * @return true on success; false otherwise
     */
    virtual bool doStopLokinetProcess() = 0;
    
    /**
     * Subclasses should provide platform-specific means of forcibly stopping
     * the lokinet process.
     *
     * @return true on success; false otherwise
     */
    virtual bool doForciblyStopLokinetProcess() = 0;

    /**
     * Subclasses should provide platform-specific means of querying the pid
     * of the lokinet process (or 0 if there is no such process)
     *
     * @param (out) pid should be filled out with the pid of the lokinet process
     *              or 0 if there is no running process.
     * @return true on success; false otherwise
     */
    virtual bool doGetProcessPid(int& pid) = 0;

private:

    /**
     * Return the last known status if it is "recent"
     */
    ProcessStatus getLastKnownStatus();

    /**
     * Update the last known status and its timestamp
     */
    void setLastKnownStatus(ProcessStatus status);

    ProcessStatus m_lastKnownStatus = ProcessStatus::Unknown;
    std::chrono::system_clock::time_point m_lastStatusTime;

    std::mutex m_managedStopMutex; // prevents more than one concurrent call to managedStopLokinetProcess()
    std::atomic_bool m_managedThreadRunning;
};
 
#endif // __LOKI_LOKINET_PROCESS_MANAGER_HPP__
