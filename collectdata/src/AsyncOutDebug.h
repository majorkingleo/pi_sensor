/*
 * AsyncOutDebug.h
 *
 * Async (background-thread) debug message frontend and backend base.
 * The frontend implements the project's local Tools::Debug interface
 * (single std::string overload) and fans messages out to backends via
 * Tools::FastDelivery.
 */
#pragma once

#include <list>
#include <string>
#include <mutex>
#include <semaphore>
#include <chrono>

#include "debug.h"          // local Tools::Debug + Tools::x_debug
#include "colored_output.h" // local ColoredOutput (terminal color detection)
#include <FastDelivery.h>   // Tools::FastDelivery::Publisher / PublisherNode

namespace AsyncOut {

struct Data
{
    const char                              *file;
    unsigned                                line;
    const char                              *function;
    std::string                             message;
    std::chrono::system_clock::time_point   when = std::chrono::system_clock::now();
};

class Logger : public Tools::FastDelivery::PublisherNode<Data>, public ColoredOutput
{
    std::list<value_type>       messages;
    std::mutex                  m_messages;
    std::counting_semaphore<>   m_worktodo{0};

public:
    void deliver( const value_type & msg );

    virtual void log();

    void wait();
    void wait_for( std::chrono::steady_clock::duration timeout );

protected:
    std::list<value_type> popAll();
};

class Debug : public Tools::Debug, public Tools::FastDelivery::Publisher<Data, Logger>
{
public:
    Debug();

    // implements the local Tools::Debug interface
    virtual void add( const char *file, unsigned line, const char *function, const std::string & s ) override;
};

} // namespace AsyncOut

