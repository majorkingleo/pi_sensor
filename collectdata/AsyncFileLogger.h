/*
 * AsyncFileLogger.h
 *
 * Async file backend: writes queued debug messages to a log file.
 * The file is size-capped; when a line would push it past max_size the
 * current file is rotated to <file>.1 and a fresh file is started.
 */
#pragma once

#include "AsyncOutDebug.h"
#include <fstream>
#include <mutex>
#include <cstdint>

namespace AsyncOut {

inline constexpr std::uintmax_t kDefaultMaxLogSize = 10 * 1024 * 1024;  // 10 MB

class FileLogger : public AsyncOut::Logger
{
    std::string       m_filename;
    std::ofstream     m_file;
    std::mutex        m_file_mutex;
    std::uintmax_t    m_max_size;
    std::uintmax_t    m_written;

public:
    explicit FileLogger( const std::string & filename_,
                         std::uintmax_t max_size_ = kDefaultMaxLogSize );

    void log() override;

    void flush();

private:
    void rotate();
};

} // namespace AsyncOut

