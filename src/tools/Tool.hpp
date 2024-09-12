#pragma once

#include <fstream>
#include <random>
#include <string>

class Tool
{
  public:
    static int LoadTexture(std::string path);

    static Tool &GetInstance()
    {
        static Tool instance;
        return instance;
    }

    template <class T> static void WriteToFile(const std::string &path, const std::vector<T> &content)
    {
        std::ofstream outfile{path, std::ios::binary};
        outfile.write(reinterpret_cast<const char *>(content.data()), content.size() * sizeof(T));
        outfile.close();
    }

    template <class T> static void ReadFromFile(const std::string &path, std::vector<T> &content)
    {
        std::ifstream infile{path, std::ios::binary};
        infile.read(reinterpret_cast<char *>(content.data()), content.size() * sizeof(T));
        infile.close();
    }

    [[nodiscard]] float RandomFloat()
    {
        return distrib_(gen_);
    }

  private:
    Tool() : gen_(rd_()), distrib_(0.0f, 1.0f)
    {
    }
    ~Tool() = default;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<float> distrib_;
};