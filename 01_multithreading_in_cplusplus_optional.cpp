// 01_multithreading_in_cplusplus_optional.cpp

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

enum class Errors
{
    // TODO: Exception handling needs to be done
    WrongParameters = 1,
    TargetFileExists,
    ErrorOpeningFile
};

constexpr std::size_t BUFFER_SIZE = 1024 * 1024;
std::mutex cerr_mutex;

void file_copy_by_parts(std::ifstream& src, std::ofstream& trgt, const std::size_t start, const std::size_t length, std::mutex& file_mutex)
{
    std::vector<char> buffer(BUFFER_SIZE);
    std::size_t length_buffer = length;

    src.seekg(start);
    trgt.seekp(start);

    while (length_buffer > 0)
    {
        std::size_t file_size_buffer_for_copy = std::min(BUFFER_SIZE, length_buffer);
        src.read(buffer.data(), file_size_buffer_for_copy);
        if (src.gcount() == 0)
            break;

        std::lock_guard<std::mutex> lock(file_mutex);
        trgt.write(buffer.data(), src.gcount());

        length_buffer -= src.gcount();
    }
}

int file_copy(const std::string& source, const std::string& target, const std::size_t num_threads)
{
    std::ifstream src(source, std::ios::binary);
    if (!src)
    {
        std::lock_guard<std::mutex> lock(cerr_mutex);
        std::cerr << "Error with opening source file.\n";
        return static_cast<int>(Errors::ErrorOpeningFile);
    }

    std::ofstream trgt(target, std::ios::binary | std::ios::app);
    if (!trgt)
    {
        std::lock_guard<std::mutex> lock(cerr_mutex);
        std::cerr << "Error with opening target file.\n";
        return static_cast<int>(Errors::ErrorOpeningFile);
    }

    src.seekg(0, src.end);
    std::size_t file_size = src.tellg();

    std::size_t filepart_size = file_size / num_threads;
    std::vector<std::thread> threads;
    std::mutex file_mutex;

    for (std::size_t i = 0; i < num_threads; ++i)
    {
        std::size_t start = i * filepart_size;
        std::size_t length = (i == num_threads - 1) ? (file_size - start) : filepart_size;
        threads.emplace_back(file_copy_by_parts, std::ref(src), std::ref(trgt), start, length, std::ref(file_mutex));
    }

    for (auto& thread : threads)
        if (thread.joinable())
            thread.join();

    return 0;
}

int main(const int argc, char* argv[])
{
    if (argc != 4)
    {
        std::lock_guard<std::mutex> lock(cerr_mutex);
        std::cerr << "Parameters of the function " << argv[0] << " should be: <source> <target> <overwrite_flag>.\n";
        return static_cast<int>(Errors::WrongParameters);
    }

    std::string source = argv[1];
    std::string target = argv[2];
    bool overwrite_flag = std::stoi(argv[3]);
    std::size_t num_threads = std::thread::hardware_concurrency();

    if (std::filesystem::exists(target) && !overwrite_flag)
    {
        std::lock_guard<std::mutex> lock(cerr_mutex);
        std::cerr << "Target file exists. To overwrite, set the overwrite flag to 1.\n";
        return static_cast<int>(Errors::TargetFileExists);
    }

    if (int result = file_copy(source, target, num_threads))
        return result;

    return 0;
}
