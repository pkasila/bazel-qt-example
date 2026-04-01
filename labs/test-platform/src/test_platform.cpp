#include "test_platform.hpp"
#include "resource_manager.hpp"
#include <filesystem>

void TestPlatform::loadTestsList() {
    if(!std::filesystem::exists("tests") || !std::filesystem::is_directory("tests")) {
        return;
    }

    for(const auto& dirEntry : std::filesystem::directory_iterator("tests")) {
        std::filesystem::path path = dirEntry.path();
        if(!std::filesystem::is_directory(path)) {
            continue;
        }
        if(!std::filesystem::exists(path / "test.toml")) {
            continue;
        }

        Test* test = new Test(this);
        if(test->load(path)) {
            tests.push_back(test);
        }
    }

    emit testsChanged();
}
