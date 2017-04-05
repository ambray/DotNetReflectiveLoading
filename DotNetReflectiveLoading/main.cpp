#include <iostream>
#include "clr.hpp"

void callback0(const wchar_t* p)
{
    std::wcout << L"Callback 0: Message: " << p << std::endl;
}

void callback1(const wchar_t* p)
{
    std::wcout << L"Callback 1: " << p << std::endl;
    auto tmp = *((char*)0);
}


class CppClassEx {
public:
    int fn() {
        return 10;
    }

    int excepted(const wchar_t* p) {
        std::wcout << L"Hello from unmanaged C++ land! " << p << std::endl;
        throw std::runtime_error("Failsauce!");
        return 20;
    }
};

int wmain(int argc, wchar_t** argv)
{
    DWORD bytes = 0;
    std::vector<uint8_t> vec;
        
    auto hf = CreateFileW(L"DemoAssembly.dll", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
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
    
    res->invoke_static(L"DemoAssembly.Demo", L"Popup", L"Hah, Messagebox!");

    auto cls = res->construct(L"DemoAssembly.Demo");
    if(!cls) {
        std::cout << "Class construction failed!" << std::endl;
        return -3;
    }

    LONGLONG fp = (LONGLONG)callback0;
    cls->invoke_method(L"Callback", fp);
    auto var = cls->invoke_method(L"GetString");
    auto bst = static_cast<bstr_t>(var);
    std::wcout << L"Message from CLR: " << static_cast<const wchar_t*>(bst) << std::endl;

    fp = (LONGLONG)callback1;
    cls->invoke_method(L"ExceptionCallback", fp);

    CppClassEx example;

    auto r = cls->invoke_method(L"AddClass", ((LONGLONG)&example));
    if (!static_cast<bool>(r))
        return -4;

    union {
        decltype(&CppClassEx::fn) f;
        decltype(&CppClassEx::excepted) ex;
        void* p;
    } u;

    u.f = &CppClassEx::fn;

    cls->invoke_method(L"InvokeMethod", ((LONGLONG)(u.p)));

    u.ex = &CppClassEx::excepted;
    cls->invoke_method(L"InvokeExcepted", ((LONGLONG)u.p));

    system("pause");
    return 0;
}