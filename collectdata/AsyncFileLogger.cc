#include "AsyncFileLogger.h"
#include <stderr_exception.h>
#include <format.h>
#include <filesystem>
#include <system_error>
#include <ctime>

using namespace AsyncOut;
using namespace Tools;

FileLogger::FileLogger( const std::string & filename_, std::uintmax_t max_size_ )
: m_filename(filename_),
  m_max_size(max_size_),
  m_written(0)
{
    m_file.open( m_filename, std::ios::out | std::ios::app );

    if( !m_file ) {
        throw STDERR_EXCEPTION( Tools::format( "cannot open log file '%s'", m_filename ) );
    }

    // Account for an existing file so the cap also holds across restarts.
    std::error_code ec;
    const auto size = std::filesystem::file_size( m_filename, ec );
    if( !ec ) {
        m_written = size;
    }
}

void FileLogger::rotate()
{
    m_file.close();

    std::error_code ec;
    std::filesystem::rename( m_filename, m_filename + ".1", ec );

    // Fallback for platforms where rename() does not replace an existing target.
    if( ec ) {
        std::filesystem::remove( m_filename + ".1", ec );
        std::filesystem::rename( m_filename, m_filename + ".1", ec );
    }

    m_file.open( m_filename, std::ios::out | std::ios::trunc );
    m_written = 0;
}

void FileLogger::log()
{
    auto lock = std::lock_guard(m_file_mutex);

    auto all_messages = popAll();

    for( const auto & m : all_messages ) {

        const std::time_t tt = std::chrono::system_clock::to_time_t( m.when );
        std::tm tm {};
        localtime_r( &tt, &tm );

        char timestamp[32];
        std::strftime( timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm );

        std::string line;
        line += '[';
        line += timestamp;
        line += "] ";
        line += std::filesystem::path(m.file).filename().string();
        line += ':';
        line += std::to_string(m.line);
        line += ' ';
        line += m.message;
        line += '\n';

        // Rotate before writing if this line would push the file over the cap.
        if( m_max_size > 0 && m_written > 0 && m_written + line.size() > m_max_size ) {
            rotate();
        }

        m_file << line;
        m_written += line.size();
    }
}

void FileLogger::flush()
{
    auto lock = std::lock_guard(m_file_mutex);
    m_file.flush();
}

