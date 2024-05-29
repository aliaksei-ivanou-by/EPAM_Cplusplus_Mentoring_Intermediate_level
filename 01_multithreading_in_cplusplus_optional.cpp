// 01_multithreading_in_cplusplus_optional.cpp

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>

enum class Errors
{
    // TODO: Exception handling needs to be done
    WrongParameters = 1,
    TargetFileExists,
    ErrorOpeningFile
};

constexpr std::size_t BUFFER_SIZE = 1024 * 1024;
std::mutex cerr_mutex;
std::mutex mutex;

class CopyQueue
{
public:
    CopyQueue() : m_end(false)
    {}
    CopyQueue(std::ifstream& src) : m_end(false)
    {
        src.seekg(0, src.end);
        m_file_size = src.tellg();
        m_file_current_size = src.tellg();
        src.seekg(0, src.beg);
    }
    void add_to_queue(std::vector<char> values)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(values);
        m_cond.notify_one();
    }
    bool remove_from_queue(std::vector<char>& values)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty() && !m_end)
            m_cond.wait(lock);
        if (m_queue.empty())
            return false;
        values = m_queue.front();
        m_queue.pop();
        return true;
    }
    void end()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_end = true;
        m_cond.notify_all();
    }
    size_t get_file_size()
    {
        return m_file_size;
    }
    int get_file_current_size()
    {
        return m_file_current_size;
    }
    void set_file_current_size(size_t value)
    {
        m_file_current_size = value;
    }
    void set_fileparts(int value)
    {
        m_fileparts = value;
    }
    int get_fileparts()
    {
        return m_fileparts;
    }
private:
    std::queue<std::vector<char>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_end;
    std::atomic<int> m_fileparts;
    int m_file_size;
    int m_file_current_size;
};

void reader(std::ifstream& src, CopyQueue& queue, size_t buffer_size)
{
    std::lock_guard<std::mutex> lock(mutex); // It's needed to fix parallel work
    while (!src.eof() && queue.get_fileparts() > 0)
    {
        std::vector<char> buffer(buffer_size);
        src.read(buffer.data(), buffer_size);
        if (src.gcount() < buffer_size)
            buffer.resize(src.gcount());
        queue.set_file_current_size(queue.get_file_current_size() - buffer.size());
        queue.set_fileparts(queue.get_fileparts() - 1);
        queue.add_to_queue(buffer);
        if (queue.get_fileparts() <= 0)
            break;
    }
    queue.end();
}

void writer(std::ofstream& trgt, CopyQueue& queue)
{
    std::vector<char> buffer;
    while (queue.remove_from_queue(buffer))
        trgt.write(buffer.data(), buffer.size());
}

int main(const int argc, char* argv[])
{
    if (argc != 4)
    {
        std::unique_lock<std::mutex> lock_cerr(cerr_mutex);
        std::cerr << "Parameters of the function " << argv[0] << " should be: <source> <target> <overwrite_flag>.\n";
        lock_cerr.unlock();
        return static_cast<int>(Errors::WrongParameters);
    }

    std::string source = argv[1];
    std::string target = argv[2];
    bool overwrite_flag = std::stoi(argv[3]);
    std::size_t num_threads = std::thread::hardware_concurrency();

    if (std::filesystem::exists(target) && !overwrite_flag)
    {
        std::unique_lock<std::mutex> lock_cerr(cerr_mutex);
        std::cerr << "Target file exists. To overwrite, set the overwrite flag to 1.\n";
        lock_cerr.unlock();
        return static_cast<int>(Errors::TargetFileExists);
    }

    std::ifstream src(source, std::ios::binary);
    if (!src)
    {
        std::unique_lock<std::mutex> lock_cerr(cerr_mutex);
        std::cerr << "Error with opening source file.\n";
        lock_cerr.unlock();
        return static_cast<int>(Errors::ErrorOpeningFile);
    }

    std::ofstream trgt(target, std::ios::binary | std::ios::app);
    if (!trgt)
    {
        std::unique_lock<std::mutex> lock_cerr(cerr_mutex);
        std::cerr << "Error with opening target file.\n";
        lock_cerr.unlock();
        return static_cast<int>(Errors::ErrorOpeningFile);
    }

    CopyQueue copy_queue(src);
    copy_queue.set_fileparts(std::ceil(static_cast<double>(copy_queue.get_file_size()) / BUFFER_SIZE));

    std::vector<std::thread> read_threads;
    for (size_t i = 0; i < num_threads; ++i)
        read_threads.emplace_back(reader, std::ref(src), std::ref(copy_queue), BUFFER_SIZE);

    std::vector<std::thread> write_threads;
    for (size_t i = 0; i < num_threads; ++i)
        write_threads.emplace_back(writer, std::ref(trgt), std::ref(copy_queue));

    for (auto& rt : read_threads)
        rt.join();

    copy_queue.end();

    for (auto& wt : write_threads)
        wt.join();

    return 0;
}
