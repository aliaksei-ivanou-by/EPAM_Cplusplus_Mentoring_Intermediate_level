// 01_multithreading_in_cplusplus_optional.cpp

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <atomic>
#include <filesystem>

std::atomic<bool> done(false);
std::vector<char> buffer;

enum class Errors
{
    Wrong_Parameters = 1,
    Target_File_Exists
};

void reader(const std::string& source)
{
    std::ifstream src(source, std::ios::binary);
    buffer = std::vector<char>((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());
    done = true;
}

void writer(const std::string& target)
{
    std::ofstream trgt(target, std::ios::binary);
    while (!done)
	{
        std::this_thread::yield();
    }
    trgt.write(buffer.data(), buffer.size());
}

int main(int argc, char* argv[])
{
    if (argc != 3)
	{
        std::cerr << "Parameters of the function " << argv[0] << " should be: <source> <target>.\n";
        return static_cast<int>(Errors::Wrong_Parameters);
    }

    std::string source = argv[1];
    std::string target = argv[2];

    if (std::filesystem::exists(target))
	{
        // TODO: Need to add flag processing
        std::cerr << "Target file exists.\n";
        return static_cast<int>(Errors::Target_File_Exists);
    }

    std::thread t1(reader, source);
    std::thread t2(writer, target);

    t1.join();
    t2.join();

    return 0;
}
