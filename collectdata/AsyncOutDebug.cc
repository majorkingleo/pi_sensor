#include "AsyncOutDebug.h"
#include <filesystem>
#include <iostream>

using namespace Tools;

AsyncOut::Debug::Debug()
: Tools::Debug()
{
}

void AsyncOut::Debug::add( const char *file, unsigned line, const char *function, const std::string & s )
{
    distribute( Data{file, line, function, s} );
}

void AsyncOut::Logger::deliver( const value_type & msg )
{
    std::lock_guard<std::mutex> ml(m_messages);
    messages.push_back( msg );
    m_worktodo.release();
}

void AsyncOut::Logger::wait()
{
    m_worktodo.acquire();
}

void AsyncOut::Logger::wait_for( std::chrono::steady_clock::duration timeout )
{
    m_worktodo.try_acquire_for(timeout);
}

std::list<AsyncOut::Logger::value_type> AsyncOut::Logger::popAll()
{
    std::lock_guard<std::mutex> ml(m_messages);
    std::list<value_type> ret( std::move_iterator(std::begin(messages)),
                               std::move_iterator(std::end(messages)) );
    messages.clear();
    return ret;
}

void AsyncOut::Logger::log()
{
    auto all_messages = popAll();

    for( const auto & m : all_messages ) {
        std::string file_name = std::filesystem::path(m.file).filename().string();
        std::cout << file_name << ':' << m.line << " " << m.message << '\n';
    }
}

