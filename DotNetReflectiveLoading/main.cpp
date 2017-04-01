#include <iostream>
#include "clr.hpp"


int wmain(int argc, wchar_t** argv)
{
    DWORD bytes = 0;
    std::vector<uint8_t> vec;
    if (argc < 3) {
        std::cout << "Please use:\nDotNetReflectiveLoading.exe <assembly_path> <class_name>" << std::endl;
    }
        
    auto hf = CreateFileW(argv[1], GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (INVALID_HANDLE_VALUE == hf)
        return 0;

    auto size = GetFileSize(hf, nullptr);
    if (size == INVALID_FILE_SIZE)
        return -1;
    vec.resize(size, 0x00);

    if (!ReadFile(hf, vec.data(), size, &bytes, nullptr) || bytes == 0) {
        std::cout << "Something went wrong! " << GetLastError() << std::endl;
        return -1;
    }

    CloseHandle(hf);

    clr::ClrDomain dom;

    auto res = dom.load(vec);
    if (!res) {
        std::cout << "Failed to load module!" << std::endl;
        return -2;
    }

    res->invoke_static(L"TestLib.Class1", L"drop_file", L"derp.txt", L"Herp Contents");

    auto cls = res->construct(argv[2]);
    if(!cls) {
        std::cout << "Class construction failed!" << std::endl;
        return -3;
    }

    cls->invoke_method(L"mb", L"This is a test");

    system("pause");
    return 0;
}