// 01_multithreading_in_cplusplus_optional.cpp

#include <atomic>
#include <condition_variable>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <queue>

enum class Errors
{
    WrongParameters = 1,
    TargetFileExists,
    ErrorOpeningFile
};

void sendErrorMessage(const char* format, ...)
{
    static std::mutex cerr_mutex;
    std::lock_guard<std::mutex> lock_cerr(cerr_mutex);
    try
    {
        va_list args_format;
        va_start(args_format, format);
        va_list args;
        va_copy(args, args_format);

        const int length = std::vsnprintf(nullptr, 0, format, args_format);
        va_end(args_format);

        std::vector<char> buf(length + 1);
        std::vsnprintf(buf.data(), buf.size(), format, args);
        va_end(args);

        std::cerr << "Error: " << buf.data() << "\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "Formatting error: " << error.what() << "\n";
    }
}

constexpr std::size_t BUFFER_SIZE = 1024 * 1024;
constexpr std::size_t MAX_BLOCKS = 4;

class CopyQueue
{
public:
    CopyQueue():
        m_availableBuffers{ { MAX_BLOCKS, std::vector<char>(BUFFER_SIZE) }  }	
    {}
    void add_buffer_to_write_queue(std::vector<char> values, std::size_t size)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queueToWrite.push(std::make_pair(std::move(values), size));
        m_cond_empty.notify_one();
    }
    std::pair<std::vector<char>, std::size_t> get_buffer_from_write_queue()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond_empty.wait(lock, [this]
            {
                return !m_queueToWrite.empty();
            });
        std::pair<std::vector<char>, std::size_t> buffer = std::move(m_queueToWrite.front());
        m_queueToWrite.pop();
        m_cond_full.notify_one();
        return buffer;
    }
    std::vector<char> get_available_buffer()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond_full.wait(lock, [this]
            {
                return !m_availableBuffers.empty();
            });
        std::vector<char> buffer = std::move(m_availableBuffers.front());
        m_availableBuffers.pop();
        return buffer;
    }
    void return_available_buffer(std::vector<char> buffer)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_availableBuffers.push(std::move(buffer));
        m_cond_full.notify_one();
    }
private:
    std::queue<std::pair<std::vector<char>, std::size_t>> m_queueToWrite;
    std::queue<std::vector<char>> m_availableBuffers;
    std::mutex m_mutex;
    std::condition_variable m_cond_empty;
    std::condition_variable m_cond_full;
};

void reader(std::ifstream& src, CopyQueue& queue)
{
    while (!src.eof())
    {
        std::vector<char> buffer = queue.get_available_buffer();
        src.read(buffer.data(), BUFFER_SIZE);
        queue.add_buffer_to_write_queue(std::move(buffer), src.gcount());
    }
    queue.add_buffer_to_write_queue(std::vector<char>(), 0);
}

void writer(std::ofstream& trgt, CopyQueue& queue)
{
    while (true)
    {
        auto [values, size] = queue.get_buffer_from_write_queue();
        if (values.empty())
            break;
        trgt.write(values.data(), size);
        queue.return_available_buffer(std::move(values));
    }
}

int main(const int argc, char* argv[])
{
    if (argc != 4)
    {
        sendErrorMessage("Parameters of the function %s should be: <source> <target> <overwrite_flag>.", argv[0]);
        return static_cast<int>(Errors::WrongParameters);
    }

    std::string source = argv[1];
    std::string target = argv[2];
    bool overwrite_flag = std::stoi(argv[3]);

    if (std::filesystem::exists(target) && !overwrite_flag)
    {
        sendErrorMessage("Target file exists. To overwrite, set the overwrite flag to 1.");
        return static_cast<int>(Errors::TargetFileExists);
    }

    std::ifstream src(source, std::ios::binary);
    if (!src)
    {
        sendErrorMessage("Error with opening source file.");
        return static_cast<int>(Errors::ErrorOpeningFile);
    }

    std::ofstream trgt(target, std::ios::binary | std::ios::trunc);
    if (!trgt)
    {
        sendErrorMessage("Error with opening target file.");
        return static_cast<int>(Errors::ErrorOpeningFile);
    }

    CopyQueue copy_queue;

    std::thread reading_thread(reader, std::ref(src), std::ref(copy_queue));
    std::thread writing_thread(writer, std::ref(trgt), std::ref(copy_queue));

    reading_thread.join();
    writing_thread.join();

    return 0;
}
